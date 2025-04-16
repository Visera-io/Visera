module;
#include <Visera.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
export module Visera.Core.Media.Model;
#define VE_MODULE_NAME "Model"
import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.OS.Memory;

export namespace VE
{
    class FModel
    {
    public:
        auto inline
        IsExpired() const -> Bool { return Data == nullptr; }
        void inline
        Load();
        void inline
        Release() { Importer.FreeScene(); }
        
        auto inline
        GetData() const -> const  aiScene* { return Data; }
        Bool inline
        HasMesh()   const   { return Data->HasMeshes(); }
        auto inline
        GetMesh(UInt32 _MeshID) const -> aiMesh* const { return Data->mMeshes[_MeshID]; }
        auto inline
        GetMeshes() const -> aiMesh** const { return Data->mMeshes; }
        auto inline
        GetMeshCount() const -> UInt32 { return Data->mNumMeshes; }
        Bool inline
        HasMaterials() const { return Data->HasMaterials(); }
        auto inline
        GetMaterials() const -> aiMaterial** const { return Data->mMaterials; }
        auto inline
        GetMaterialCount() const -> UInt32 { return Data->mNumMaterials; }

        FModel() = delete;
        FModel(const FPath& _Path);
        ~FModel() = default;

    private:
        const FPath      Path;
        Assimp::Importer Importer;
        const aiScene*   Data{nullptr};
    };

    FModel::
    FModel(const FPath& _Path) : Path(_Path)
    {
        Load();
    }

    void inline
    FModel::
    Load()
    {
        const String PlatformPath = Path.ToPlatformString();
        VE_LOG_DEBUG("Loading a new model from {}", PlatformPath);

        if (!IsExpired())
        { return VE_LOG_WARN("You tried to load an unexpired model({})!", PlatformPath); }

        Data = Importer.ReadFile(PlatformPath.data(),
                aiProcess_Triangulate	    |
                aiProcess_ConvertToLeftHanded   | // <=> MakeLeftHanded | FlipUVs | FlipWindingOrder
                aiProcess_GenNormals            |
                aiProcess_FixInfacingNormals);

        if (!Data            ||
            !Data->mRootNode ||
             Data->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            String ErrorInfo = Text("Failed to load the model({})! -- throw (SIOFailure).", PlatformPath);
			VE_LOG_ERROR("{}", ErrorInfo);
            throw SIOFailure(ErrorInfo);
        }

        if (!HasMesh())
        { VE_LOG_WARN("The model {} has 0 mesh!", Path.ToPlatformString()); }
    }
}