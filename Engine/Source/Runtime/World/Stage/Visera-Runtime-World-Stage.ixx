module;
#include <Visera.h>

export module Visera.Runtime.World.Stage;
import Visera.Runtime.World.Stage.Scene;
import Visera.Runtime.World.Object;

import Visera.Core.System;
import Visera.Core.Log;

export namespace VE { namespace Runtime
{

	class VStage : public VObject
	{
	public:
		virtual void Update()  override;
		virtual void Create()  override;
		virtual void Destroy() override;

		// Don't forget to Update() Stage after modification.
		Bool AttachScene(FName _SceneName, const String& _SceneFile);
		Bool RemoveScene(FName _SceneName);

		VStage() = delete;
		VStage(FName _StageName, const String& _StageFile);
		~VStage();

	private:
		HashMap<FName, SharedPtr<VScene>> Scenes;
	};

	Bool VStage::
	AttachScene(FName _SceneName, const String& _SceneFile)
	{
		Bool Result = False;
		RWLock.StartWriting();
		{
			auto& SceneSlot = Scenes[_SceneName];
			if (SceneSlot == nullptr)
			{
				SceneSlot = CreateSharedPtr<VScene>(_SceneName, _SceneFile);
				Result = True;
			}
			else
			{
				Log::Warn("You may attach a duplicated Scene({}) to the Stage({})."
						  "-- If you want to do that, try to increase the number of FName.",
						  GetObjectNameWithNumber(), SceneSlot->GetObjectName());
				Result = False;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	Bool VStage::
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
				Log::Warn("Failed to remove Scene({}) from Stage({})", _SceneName.GetNameWithNumber(), GetObjectNameWithNumber());
				Result = False;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	void VStage::
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

	void VStage::
	Create()
	{
		RWLock.StartWriting();
		{
			
		}
		RWLock.StopWriting();
	}

	void VStage::
	Destroy()
	{
		RWLock.StartWriting();
		{
			
		}
		RWLock.StopWriting();
	}

	VStage::
	VStage(FName _StageName, const String& _StageFile) : VObject(_StageName)
	{

	}

	VStage::
	~VStage()
	{

	}

} } // namespace VE::Runtime