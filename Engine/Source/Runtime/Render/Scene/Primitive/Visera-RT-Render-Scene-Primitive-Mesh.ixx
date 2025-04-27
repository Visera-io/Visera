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
        using FUVCoord  = Vector3F;

        struct FVertex
        {
            alignas(16) FPosition Position;
            alignas(16) FNormal   Normal;
            alignas(16) FUVCoord  TextureCoord;
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

        void inline //[FIXME]: if No IBO
        FlipFaceWinding() { for (size_t i = 0; i < Indices.size(); i += 3) {std::swap(Indices[i + 1], Indices[i + 2]); } }

        FMeshPrimitive() = delete;
        FMeshPrimitive(SharedPtr<const FModel> _Model);

    private:
        Array<FVertex>  Vertices;
        Array<FIndex>   Indices;
    };

    FMeshPrimitive::
    FMeshPrimitive(SharedPtr<const FModel> _Model) //[TODO]: pass FModel::FMeshModel?
        : IPrimitive{ _Model }
    {
        const auto Meshes = _Model->GetMeshes();
        for (UInt32 MeshID = 0; MeshID < _Model->GetMeshCount(); ++MeshID)
        {
            const auto Mesh = Meshes[MeshID];

            const UInt64 OldVertexCount = Vertices.size();
            Vertices.resize(OldVertexCount + Mesh->mNumVertices);
            for (UInt64 Idx = 0; Idx < Mesh->mNumVertices; ++Idx)
            {
                FVertex& CurrentVertex = Vertices[OldVertexCount + Idx];
                Memory::Memcpy(&CurrentVertex.Position, &(Mesh->mVertices[Idx]), sizeof(FPosition));
                BoundingBox.MaxPosition = GetComponentWiseMax(BoundingBox.MaxPosition, CurrentVertex.Position);
                BoundingBox.MinPosition = GetComponentWiseMin(BoundingBox.MinPosition, CurrentVertex.Position);

                if (Mesh->HasNormals())
                { Memory::Memcpy(&CurrentVertex.Normal, &(Mesh->mNormals[Idx]), sizeof(FNormal)); }

                if (Mesh->HasTextureCoords(0))
                {
                    const auto& TextureCoord = Mesh->mTextureCoords[0][Idx];
                    Memory::Memcpy(&CurrentVertex.TextureCoord, &TextureCoord, sizeof(FUVCoord));
                }
            }

            const UInt64 OldIndexCount = Indices.size();
            Indices.resize(OldIndexCount + 3 * Mesh->mNumFaces);
            for (UInt64 Idx = 0; Idx < Mesh->mNumFaces; Idx++)
            {
                const auto Face = Mesh->mFaces[Idx];
                VE_ASSERT(Face.mNumIndices == 3);
                const UInt64 Offset = OldIndexCount + Idx * 3;
                Indices[Offset + 0] = Face.mIndices[0] + OldVertexCount;
                Indices[Offset + 1] = Face.mIndices[1] + OldVertexCount;
                Indices[Offset + 2] = Face.mIndices[2] + OldVertexCount;
            }
        }
        BoundingBox.Center = (BoundingBox.MaxPosition + BoundingBox.MinPosition) / 2.0;
    }

}// namespace VE