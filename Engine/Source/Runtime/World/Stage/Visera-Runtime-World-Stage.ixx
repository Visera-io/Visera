module;
#include <Visera.h>
export module Visera.Runtime.World.Stage;

import Visera.Core.OS;
import Visera.Core.Log;
import Visera.Core.Type;

export namespace VE
{

	// class FStage
	// {
	// public:
	// 	void Update() ;
	// 	void Create() ;
	// 	void Destroy();
	//
	// 	// Don't forget to Update() Stage after modification.
	// 	Bool AttachScene(FName _SceneName, const String& _SceneFile);
	// 	Bool DetachScene(FName _SceneName);
	// 	auto SearchScene(FName _SceneName) -> SharedPtr<FScene>;
	//
	// 	FStage() = delete;
	// 	FStage(FName _Name, const String& _StageFile);
	// 	~FStage();
	//
	// private:
	// 	mutable FRWLock RWLock;
	// 	FName   Name;
	// 	SharedPtr<VCamera>				  Camera;
	// 	HashMap<FName, SharedPtr<FScene>> Scenes;
	// };
	//
	// Bool FStage::
	// AttachScene(FName _SceneName, const String& _SceneFile)
	// {
	// 	Bool Result = False;
	// 	RWLock.StartWriting();
	// 	{
	// 		SharedPtr<FScene>& SceneSlot = Scenes[_SceneName];
	// 		if (SceneSlot == nullptr)
	// 		{
	// 			SceneSlot = CreateSharedPtr<FScene>(_SceneName, _SceneFile);
	// 			Result = True;
	// 		}
	// 		else
	// 		{
	// 			Log::Warn("You may attach a duplicated Scene({}) to the Stage({})."
	// 					  "-- If you want to do that, try to increase the number of FName.",
	// 					  Name.GetNameWithNumber(), SceneSlot->GetFileName().GetNameWithNumber());
	// 			Result = False;
	// 		}
	// 	}
	// 	RWLock.StopWriting();
	//
	// 	return Result;
	// }
	//
	// Bool FStage::
	// DetachScene(FName _SceneName)
	// {
	// 	Bool Result = False;
	// 	RWLock.StartWriting();
	// 	{
	// 		auto& SceneSlot = Scenes[_SceneName];
	// 		if (SceneSlot != nullptr)
	// 		{
	// 			SceneSlot.reset();
	// 			Result = True;
	// 		}
	// 		else
	// 		{
	// 			Log::Warn("Failed to remove Scene({}) from Stage({})",
	// 					  _SceneName.GetNameWithNumber(), Name.GetNameWithNumber());
	// 			Result = False;
	// 		}
	// 	}
	// 	RWLock.StopWriting();
	//
	// 	return Result;
	// }
	//
	// SharedPtr<FScene> FStage::
	// SearchScene(FName _SceneName)
	// {
	// 	SharedPtr<FScene> Result;
	//
	// 	RWLock.StartReading();
	// 	{
	// 		auto Target = Scenes.find(_SceneName);
	// 		if (Target != Scenes.end())
	// 		{
	// 			Result = Target->second;
	// 		}
	// 	}
	// 	RWLock.StopReading();
	//
	// 	return Result;
	// }
	//
	// void FStage::
	// Update()
	// {
	// 	RWLock.StartWriting();
	// 	{
	// 		for (auto& [Name, Scene] : Scenes)
	// 		{
	// 			if (Scene) { Scene->Update(); }
	// 		}
	// 	}
	// 	RWLock.StopWriting();
	// }
	//
	// void FStage::
	// Create()
	// {
	// 	RWLock.StartWriting();
	// 	{
	// 		for (auto& [Name, Scene] : Scenes)
	// 		{
	// 			if (Scene) { Scene->Create(); }
	// 		}
	// 	}
	// 	RWLock.StopWriting();
	// }
	//
	// void FStage::
	// Destroy()
	// {
	// 	RWLock.StartWriting();
	// 	{
	// 		for (auto& [Name, Scene] : Scenes)
	// 		{
	// 			if (Scene) { Scene->Destroy(); }
	// 		}
	// 	}
	// 	RWLock.StopWriting();
	// }
	//
	// FStage::
	// FStage(FName _Name, const String& _StageFile) : Name(_Name)
	// {
	//
	// }
	//
	// FStage::
	// ~FStage()
	// {
	//
	// }

} // namespace VE