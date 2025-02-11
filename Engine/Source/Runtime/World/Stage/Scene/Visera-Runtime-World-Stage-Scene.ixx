module;
#include <Visera.h>
#include <embree4/rtcore.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
export module Visera.Runtime.World.Stage.Scene;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.System;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Geometry;
import Visera.Runtime.World.RTC;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{

	class FScene
	{
	public:
		auto GetHandle() const -> RTCScene		{ return Handle; }
		auto GetFileName()   const -> FName			{ return Name; }
		auto GetPath()	 const -> StringView	{ return Path; }
		auto IsLoaded()  const -> Bool			{ return Data != nullptr; }

		void Update()  ;
		void Create()  ;
		void Destroy() ;

	public:
		FScene(FName _Name, const String _FilePath);
		FScene() = delete;
		~FScene();

	private:
		FRWLock					RWLock;
		FName					Name;
		String					Path;
		RTCScene				Handle = nullptr;
		const aiScene*			Data   = nullptr;
		Array<SharedPtr<FMesh>> Meshes; //[TODO]: General Geometry

		static inline Assimp::Importer Importer;
	};

	void FScene::
	Update()
	{
		RWLock.StartWriting();
		{
			for (auto& Mesh : Meshes)
			{
				if (Mesh) Mesh->Update();
			}
			rtcCommitScene(Handle);
		}
		RWLock.StopWriting();
	}

	void FScene::Create()
	{
		if (Handle) { return; }

		RWLock.StartWriting();
		{
			Handle = rtcNewScene(RTC::GetAPI()->GetDevice());
			if (!Handle)
			{ throw SRuntimeError(Text("Failed to create Embree Scene({})!", Name.GetNameWithNumber())); }

			Log::Debug("Creating Scene {} from {}.", Name.GetNameWithNumber(), Path);
			//[TODO]: Assimp Support
			/*Data = Importer.ReadFile(Path,
				aiProcess_Triangulate		 |
				aiProcess_ConvertToLeftHanded|
				aiProcess_FixInfacingNormals
				);

			if (!Data ||
				 Data->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
				!Data->mRootNode)
			{ throw SIOFailure(Text("Failed to load scene {} from {}!", Name.GetNameWithNumber(), Path)); }*/

			//[TODO]: Search Shared Mesh
			Array<Float> Vertices(9);
			Vertices[0] = 0.f; Vertices[1] = 0.f; Vertices[2] = 0.f;
			Vertices[3] = 500.f; Vertices[4] = 0.f; Vertices[5] = 0.f;
			Vertices[6] = 0.f; Vertices[7] = 500.f; Vertices[8] = 0.f;

			Array<UInt32> Indices(3);
			Indices[0] = 0; Indices[1] = 1; Indices[2] = 2;

			auto& Mesh = Meshes.emplace_back(Geometry::CreateMesh(Geometry::FMesh::FCreateInfo
				{
					.Topology		= Geometry::FMesh::ETopology::Triangle,
					.VertexCount	= Vertices.size() / 3,
					.Vertices		= nullptr,
					.IndexCount		= Indices.size() / 3,
					.Indices		= nullptr,
				}));
			Mesh->SetVertices(Vertices.data());
			Mesh->SetIndices(Indices.data());
			rtcAttachGeometry(Handle, Mesh->GetHandle());
		}
		RWLock.StopWriting();
	}

	void FScene::Destroy()
	{		
		if (!Handle) { return; }

		RWLock.StartWriting();
		{
			Log::Debug("Destroying Scene {} from {}.", Name.GetNameWithNumber(), Path);
			
			rtcReleaseScene(Handle);
		}
		RWLock.StopWriting();
	}

	FScene::
	FScene(FName _Name, const String _FilePath)
		: Name{_Name},
		  Path{_FilePath}
	{

	}

	FScene::
	~FScene()
	{

	}

} } // namespace VE::Runtime