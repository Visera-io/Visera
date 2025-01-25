module;
#include <Visera.h>
export module Visera.Runtime.World.Stage;
import Visera.Runtime.World.Stage.Scene;
import Visera.Runtime.World.Object;

import Visera.Core.System;
import Visera.Core.Log;
import Visera.Core.Type;

export namespace VE { namespace Runtime
{

	class FStage
	{
	public:
		void Update() ;
		void Create() ;
		void Destroy();

		// Don't forget to Update() Stage after modification.
		Bool AttachScene(FName _SceneName, const String& _SceneFile);
		Bool RemoveScene(FName _SceneName);

		FStage() = delete;
		FStage(FName _Name, const String& _StageFile);
		~FStage();

	private:
		FRWLock RWLock;
		FName   Name;
		HashMap<FName, SharedPtr<FScene>> Scenes;
	};

	Bool FStage::
	AttachScene(FName _SceneName, const String& _SceneFile)
	{
		Bool Result = False;
		RWLock.StartWriting();
		{
			SharedPtr<FScene>& SceneSlot = Scenes[_SceneName];
			if (SceneSlot == nullptr)
			{
				SceneSlot = CreateSharedPtr<FScene>(_SceneName, _SceneFile);
				Result = True;
			}
			else
			{
				Log::Warn("You may attach a duplicated Scene({}) to the Stage({})."
						  "-- If you want to do that, try to increase the number of FName.",
						  Name.GetNameWithNumber(), SceneSlot->GetName().GetNameWithNumber());
				Result = False;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	Bool FStage::
	RemoveScene(FName _SceneName)
	{
		Bool Result = False;
		RWLock.StartWriting();
		{
			auto& SceneSlot = Scenes[_SceneName];
			if (SceneSlot != nullptr)
			{
				SceneSlot.reset();
				Result = True;
			}
			else
			{
				Log::Warn("Failed to remove Scene({}) from Stage({})",
						  _SceneName.GetNameWithNumber(), Name.GetNameWithNumber());
				Result = False;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	void FStage::
	Update()
	{
		RWLock.StartWriting();
		{
			for (auto& [Name, Scene] : Scenes)
			{
				if (Scene) { Scene->Update(); }
			}
		}
		RWLock.StopWriting();
	}

	void FStage::
	Create()
	{
		RWLock.StartWriting();
		{
			
		}
		RWLock.StopWriting();
	}

	void FStage::
	Destroy()
	{
		RWLock.StartWriting();
		{
			
		}
		RWLock.StopWriting();
	}

	FStage::
	FStage(FName _Name, const String& _StageFile) : Name(_Name)
	{

	}

	FStage::
	~FStage()
	{

	}

} } // namespace VE::Runtime