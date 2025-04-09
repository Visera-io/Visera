module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.Scene.Primitive:Mesh;
import :Interface;

import Visera.Core.Math.Basic;
import Visera.Core.Media.Model;
import Visera.Core.OS.Memory;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FMeshPrimitive : public IPrimitive
    {
    public:
        using FIndex    = UInt32;
        using FPosition = Vector3F;
        using FNormal   = Vector3F;
        using FUVCoord  = Vector2F;

        struct FVertex
        {
            alignas(16) FPosition Position;
            alignas(16) FNormal   Normal;
            // Array<FUVCoord> UVs;
        };

        static inline auto
        Create(SharedPtr<const FModel> _Model) { return CreateSharedPtr<FMeshPrimitive>(_Model); }
        virtual auto
        GetVertexData()      const -> const void*   override { return Vertices.data(); }
        virtual auto
        GetVertexCount()     const -> UInt64        override { return Vertices.size(); }
        virtual auto
        GetVertexByteSize()  const -> UInt64        override { return sizeof(FVertex); }

        virtual auto
        GetIndexData()       const -> const void*   override { return Indices.data(); }
         virtual auto
        GetIndexCount()      const -> UInt64        override { return Indices.size();  }
        virtual auto
        GetIndexByteSize()   const -> UInt64        override { return sizeof(FIndex);  }

        FMeshPrimitive(SharedPtr<const FModel> _Model);

    private:
        Array<FVertex>  Vertices;
        Array<FIndex>   Indices;
    };

    FMeshPrimitive::
    FMeshPrimitive(SharedPtr<const FModel> _Model) //[TODO]: pass FModel::FMeshModel?
        : IPrimitive{ _Model,
        _Model->GetMeshes()[0]->mNumVertices * sizeof(FVertex),
        _Model->GetMeshes()[0]->mNumFaces * (3 * sizeof(FIndex)) }
    {
        auto Meshes = _Model->GetMeshes();
        //for (UInt32 Idx = 0; Idx < _Model->GetMeshCount(); ++Idx)
        auto Mesh = Meshes[0];
        
        Vertices.resize(Mesh->mNumVertices);
        for (UInt64 Idx = 0; Idx < Vertices.size(); ++Idx)
        {
            auto& Position = Vertices[Idx].Position;
            Memory::Memcpy(&Position, &(Mesh->mVertices[Idx]), sizeof(FPosition));
            BoundingBox.MaxPosition = GetComponentWiseMax(BoundingBox.MaxPosition, Position);
            BoundingBox.MinPosition = GetComponentWiseMin(BoundingBox.MinPosition, Position);

            auto& Normal = Vertices[Idx].Normal;
            if (Mesh->HasNormals())
            {  Memory::Memcpy(&Normal, &(Mesh->mNormals[Idx]), sizeof(FNormal)); }

            if (Mesh->HasTextureCoords(0))
            {
                //UVs.resize(Vertices.size());
                //Memory::Memcpy(UVs.data()->data(), Mesh->mTextureCoords[0], UVs.size() * sizeof(FUVCoord));
            }
        }
        BoundingBox.Center = (BoundingBox.MaxPosition + BoundingBox.MinPosition) / 2.0;
       
        Indices.resize(3 * Mesh->mNumFaces);
        for (UInt32 Idx = 0; Idx < Mesh->mNumFaces; Idx++)
        {
            auto Face = Mesh->mFaces[Idx];
            VE_ASSERT(Face.mNumIndices == 3);
            Memory::Memcpy(Indices.data() + Face.mNumIndices * Idx,
                           Face.mIndices,
                           Face.mNumIndices * sizeof(FIndex));
        }

        //Load to GPU [TODO]: Batch upload at RHI level (Add a transfer cmdbuffer in each Frame)
        auto VertexStagingBuffer = RHI::CreateStagingBuffer(GetCPUVertexBufferSize());
        VertexStagingBuffer->Write(Vertices.data(), GetCPUVertexBufferSize());
        auto IndexStagingBuffer  = RHI::CreateStagingBuffer(GetCPUIndexBufferSize());
        IndexStagingBuffer->Write(Indices.data(), GetCPUIndexBufferSize());

        auto Fence = RHI::CreateFence();
        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer(); //[TODO]: Transfer Buffer
        ImmeCmd->StartRecording();
        {
            ImmeCmd->WriteBuffer(VBO, VertexStagingBuffer);
            ImmeCmd->WriteBuffer(IBO, IndexStagingBuffer);
        }
        ImmeCmd->StopRecording();
        ImmeCmd->Submit(RHI::FCommandSubmitInfo
            {
                .WaitStages = RHI::EGraphicsPipelineStage::PipelineTop,
                .SignalFence = &Fence
            });
        Fence.Wait();
    }

}// namespace VE