module;
#include <Visera.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
export module Visera.Core.Media.Model;

import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.OS.Memory;

export namespace VE
{
    class FModel
    {
    public:
        auto inline
        GetVertices() const -> const Array<Float>& { return Vertices; }
        auto inline
        GetVertexData() const -> const Float* { return Vertices.data(); }
        auto inline
        GetVertexCount() const -> size_t { return Vertices.size() / 3; }
        auto inline
        GetIndices() const -> const Array<UInt32>& { return Indices; }
        auto inline
        GetIndexCount() const -> size_t { return Indices.size(); }
        auto inline
        GetIndexData() const -> const UInt32* { return Indices.data(); }

        auto inline
        IsLoaded()      const -> Bool { return Data != nullptr;}
        auto inline
        HasNormals()    const -> Bool { return !Normals.empty();}
        auto inline
        HasTextureCoords() const -> Bool { return !TextureCoords.empty();}
        auto inline
        HasIndices() const -> Bool { return !Indices.empty();}

        FModel() = delete;
        FModel(const FPath& _Path);
        ~FModel() = default;

    private:
        const aiScene* Data   = nullptr;
        const FPath    Path;

        Array<Float>   Vertices;
        Array<UInt32>  Indices;
        Array<Float>   Normals;
        Array<Float>   TextureCoords;
    };

    FModel::
    FModel(const FPath& _Path) : Path(_Path)
    {
        static Assimp::Importer Importer;

        Data = Importer.ReadFile(Path.ToPlatformString().data(),
                aiProcess_Triangulate	        |
                aiProcess_ConvertToLeftHanded   |
                aiProcess_FixInfacingNormals);

        if (!Data ||
            !Data->mRootNode ||
            Data->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        { throw SRuntimeError("Failed to load Model!"); }

        if (Data->mNumMeshes > 0)
        {
            aiMesh* Mesh = Data->mMeshes[0];
            Vertices.resize(3 * Mesh->mNumVertices);
            if (Mesh->HasNormals()) Normals.resize( 3 * Mesh->mNumVertices);
            if (Mesh->HasTextureCoords(0)) TextureCoords.resize(2 * Mesh->mNumVertices);
            for (UInt32 i = 0; i < Mesh->mNumVertices; i++)
            {
                Memory::Memcpy(Vertices.data() + 3 * i, &(Mesh->mVertices[i]), sizeof(Float) * 3);

                if (Mesh->HasNormals())
                { Memory::Memcpy(Normals.data() + 3 * i, &(Mesh->mNormals[i]), sizeof(Float) * 3); }

                if (Mesh->HasTextureCoords(0))
                {
                    Memory::Memcpy(TextureCoords.data() + 2 * i, &(Mesh->mTextureCoords[0][i]), sizeof(Float) * 2);
                }
            }

            Indices.resize(3 * Mesh->mNumFaces);
            for (UInt32 i = 0; i < Mesh->mNumFaces; i++)
            {
                aiFace Face = Mesh->mFaces[i];
                VE_ASSERT(Face.mNumIndices == 3);
                Memory::Memcpy(Indices.data() + 3 * i, Face.mIndices, sizeof(UInt32) * 3);
            }
        }
        else{ throw SRuntimeError("Invalid meshes (Current Scene NOT be freed)!"); }

        Importer.FreeScene();
    }

}