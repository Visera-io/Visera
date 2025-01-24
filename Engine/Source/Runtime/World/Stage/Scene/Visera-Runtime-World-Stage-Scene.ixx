module;
#include <Visera.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
export module Visera.Runtime.World.Stage.Scene;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.System;
import Visera.Runtime.World.Object;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{

	class VScene : public VObject
	{
	public:
		auto GetPath()	const -> StringView { return Path; }
		auto IsLoaded() const -> Bool { return Handle != nullptr; }

		virtual void Update()  override;
		virtual void Create()  override;
		virtual void Destroy() override;

	public:
		VScene(FName _Name, const String _FilePath);
		VScene() = delete;
		~VScene();

	private:
		const aiScene* Handle = nullptr;
		String		   Path;

		static inline Assimp::Importer Importer;
	};

	void VScene::
	Update()
	{
		RWLock.StartWriting();
		{

		}
		RWLock.StopWriting();
	}

	void VScene::Create()
	{
		RWLock.StartWriting();
		{
			Log::Debug("Creating Scene {} from {}.", GetObjectName(), Path);
			Handle = Importer.ReadFile(Path,
				aiProcess_Triangulate		 |
				aiProcess_ConvertToLeftHanded|
				aiProcess_FixInfacingNormals
				);

			if (!Handle ||
				 Handle->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
				!Handle->mRootNode)
			{ throw SIOFailure(Text("Failed to load scene {} from {}!", GetObjectName(), Path)); }
		}
		RWLock.StopWriting();
	}

	void VScene::Destroy()
	{
		RWLock.StartWriting();
		{
			Log::Debug("Destroying Scene {} from {}.", GetObjectName(), Path);
		}
		RWLock.StopWriting();
	}

	VScene::
	VScene(FName _Name, const String _FilePath)
		: VObject(_Name),
		  Path{_FilePath}
	{

	}

	VScene::
	~VScene()
	{

	}

} } // namespace VE::Runtime