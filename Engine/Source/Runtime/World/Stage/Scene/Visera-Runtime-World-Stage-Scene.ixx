module;
#include <Visera.h>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
export module Visera.Runtime.World.Stage.Scene;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Runtime.World.Object;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{

	class VScene : public VObject
	{
	public:
		auto GetPath()	const -> StringView { return Path; }
		//auto IsLoaded() const -> Bool { return Handle != nullptr; }

		virtual void Create()  override;
		virtual void Destroy() override;

	public:
		VScene(const String& _Name, const String _FilePath);
		VScene() = delete;
		~VScene();

	private:
		//const aiScene* Handle = nullptr;
		String		   Path;

		//static inline Assimp::Importer Importer;
	};

	void VScene::Create()
	{
		Log::Debug("Creating Scene {} from {}.", GetID(), Path);
		/*Handle = Importer.ReadFile(Path,
			aiProcess_Triangulate		 |
			aiProcess_ConvertToLeftHanded|
			aiProcess_FixInfacingNormals
			);

		if (!Handle ||
			 Handle->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
			!Handle->mRootNode)
		{ throw SIOFailure(Text("Failed to load scene {} from {}!", GetID(), Path)); }*/
	}

	void VScene::Destroy()
	{
		Log::Debug("Destroying Scene {} from {}.", GetID(), Path);
	}

	VScene::
	VScene(const String& _Name, const String _FilePath)
		: VObject(_Name),
		  Path{_FilePath}
	{

	}

	VScene::
	~VScene()
	{

	}

} } // namespace VE::Runtime