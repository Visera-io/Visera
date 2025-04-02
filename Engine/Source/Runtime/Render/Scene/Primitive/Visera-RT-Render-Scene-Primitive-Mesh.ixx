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
        using FVertex = Vector3F;
        using FNormal = Vector3F;
        using FUVCoord = Vector2F;

        static inline auto
        Create(SharedPtr<const FModel> _Model) { return CreateSharedPtr<FMeshPrimitive>(_Model); }

        virtual auto inline
        GetVertexCount()     -> UInt64        override { return Vertices.size(); }
        virtual auto inline
        GetVertexByteSize()  -> UInt64        override { return sizeof(FVertex); }
        virtual auto inline
        GetVerticesData()    -> const Float*  override { return Vertices.data()->data(); }
        virtual auto inline
        GetIndexCount()      -> UInt64        override { return Indices.size(); }
        virtual auto inline
        GetIndexByteSize()  -> UInt64         override { return sizeof(FIndex); }
        virtual auto inline
        GetIndicesData()     -> const FIndex* override { return Indices.data(); }

        FMeshPrimitive(SharedPtr<const FModel> _Model);

    private:
        Array<FVertex>  Vertices;
        Array<FNormal>  Normals;
        Array<FIndex>   Indices;
        Array<FUVCoord> UVs;
    };

    FMeshPrimitive::
    FMeshPrimitive(SharedPtr<const FModel> _Model) //[TODO]: pass FModel::FMesh?
        : IPrimitive{ _Model, 
            RHI::CreateVertexBuffer(_Model->GetMeshes()[0]->mNumVertices * sizeof(FVertex) ),
            RHI::CreateIndexBuffer(_Model->GetMeshes()[0]->mNumFaces * (3 * sizeof(FIndex))) }
    {
        auto Meshes = _Model->GetMeshes();
        //for (UInt32 Idx = 0; Idx < _Model->GetMeshCount(); ++Idx)
        auto Mesh = Meshes[0];
        
        Vertices.resize(Mesh->mNumVertices);
        Memory::Memcpy(Vertices.data(), Mesh->mVertices, GetCPUVertexBufferSize());

        Indices.resize(3 * Mesh->mNumFaces);
        for (UInt32 Idx = 0; Idx < Mesh->mNumFaces; Idx++)
        {
            aiFace Face = Mesh->mFaces[Idx];
            VE_ASSERT(Face.mNumIndices == 3);
            Memory::Memcpy(Indices.data() + 3 * Idx, Face.mIndices, Face.mNumIndices * sizeof(FIndex));
        }

        if (Mesh->HasNormals())
        {
            Normals.resize(Vertices.size());
            Memory::Memcpy(Normals.data(), Mesh->mNormals, Normals.size() * sizeof(FNormal));
        }

        if (Mesh->HasTextureCoords(0))
        {
            UVs.resize(Vertices.size());
            Memory::Memcpy(UVs.data()->data(), Mesh->mTextureCoords[0], UVs.size() * sizeof(FUVCoord));
        }

        //Load to GPU [TODO]: Batch upload at RHI level (Add a transfer cmdbuffer in each Frame)
        auto VertexStagingBuffer = RHI::CreateStagingBuffer(GetCPUVertexBufferSize());
        VertexStagingBuffer->Write(Vertices.data(), GetCPUVertexBufferSize());
        auto IndexStagingBuffer  = RHI::CreateStagingBuffer(GetCPUIndexBufferSize());
        VertexStagingBuffer->Write(Indices.data(), GetCPUIndexBufferSize());

        auto Fence = RHI::CreateFence();
        auto ImmeCmd = RHI::CreateImmediateCommandBuffer(); //[TODO]: Transfer Buffer
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