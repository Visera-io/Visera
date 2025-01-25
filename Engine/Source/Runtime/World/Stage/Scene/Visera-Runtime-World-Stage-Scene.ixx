module;
#include <Visera.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
export module Visera.Runtime.World.Stage.Scene;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.System;
import Visera.Runtime.World.Object;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{

	class FScene
	{
	public:
		auto GetName()  const -> FName { return Name; }
		auto GetPath()	const -> StringView { return Path; }
		auto IsLoaded() const -> Bool { return Handle != nullptr; }

		void Update()  ;
		void Create()  ;
		void Destroy() ;

	public:
		FScene(FName _Name, const String _FilePath);
		FScene() = delete;
		~FScene();

	private:
		FRWLock		   RWLock;
		FName		   Name;
		String		   Path;
		const aiScene* Handle = nullptr;

		static inline Assimp::Importer Importer;
	};

	void FScene::
	Update()
	{
		RWLock.StartWriting();
		{

		}
		RWLock.StopWriting();
	}

	void FScene::Create()
	{
		RWLock.StartWriting();
		{
			Log::Debug("Creating Scene {} from {}.", Name.GetNameWithNumber(), Path);
			Handle = Importer.ReadFile(Path,
				aiProcess_Triangulate		 |
				aiProcess_ConvertToLeftHanded|
				aiProcess_FixInfacingNormals
				);

			if (!Handle ||
				 Handle->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
				!Handle->mRootNode)
			{ throw SIOFailure(Text("Failed to load scene {} from {}!", Name.GetNameWithNumber(), Path)); }
		}
		RWLock.StopWriting();
	}

	void FScene::Destroy()
	{
		RWLock.StartWriting();
		{
			Log::Debug("Destroying Scene {} from {}.", Name.GetNameWithNumber(), Path);
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