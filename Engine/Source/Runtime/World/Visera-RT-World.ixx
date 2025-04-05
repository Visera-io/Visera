module;
#include <Visera.h>
export module Visera.Runtime.World;
export import Visera.Runtime.World.Atlas;
export import Visera.Runtime.World.Object;
export import Visera.Runtime.World.Actor;
export import Visera.Runtime.Render.Camera;
export import Visera.Runtime.World.Stage;

import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.Math.Basic;
import Visera.Core.OS.Concurrency;
import Visera.Core.OS.FileSystem;

export namespace VE
{
	class World
	{
		VE_MODULE_MANAGER_CLASS(World);
	public:
		using VObject = VObject;

		static inline auto
		GetCoordinate() -> const FViseraChart& { return Atlas::Visera; }

		template<VObjectType T> static auto
		CreateObject(FName _Name) -> SharedPtr<T>;
		static inline auto
		SearchObject(FName _Name) -> SharedPtr<VObject>;
		// static inline auto
		// CreateStage(FName _StageName, const String& _StageFile) -> SharedPtr<FStage> { return CreateSharedPtr<FStage>(_StageName, _StageFile); }

		static inline auto Bootstrap() -> void;
		static inline auto Update() -> void;
		static inline auto Terminate() -> void;

	private:
		static inline FRWLock RWLock;
		static inline auto ObjectTable = HashMap<FName, SharedPtr<VObject>>();
	};

	void World::
	Bootstrap()
	{

	}

	void World::
	Update()
	{

	}

	void World::
	Terminate()
	{
		RWLock.StartWriting();
		{
			for (auto& [Name, Object] : ObjectTable)
			{
				Object->Destroy();
			}
		}
		RWLock.StopWriting();
	}

	template<VObjectType T>
	SharedPtr<T> World::
	CreateObject(FName _Name)
	{ 
		static_assert(VObjectType<T>, "T must satisfy the VObjectType concept!");
		SharedPtr<T> NewObject = nullptr;

		RWLock.StartWriting();
		{
			auto& ObjectSlot = ObjectTable[_Name];
			if (ObjectSlot == nullptr)
			{
				NewObject = CreateSharedPtr<T>();
				NewObject->Create();
				NewObject->Name = _Name;
				ObjectSlot      = NewObject;
			}
			else
			{
				Log::Warn("You may create a duplicated Object({}) to the World."
						  "-- If you want to do that, try to increase the number of FName.",
						  _Name.GetNameWithNumber()); 
			}
		}
		RWLock.StopWriting();

		return NewObject;
	}

	SharedPtr<VObject> 
	World::SearchObject(FName _Name)
	{
		SharedPtr<VObject> Result = nullptr;

		RWLock.StartReading();
		{
			auto Target = ObjectTable.find(_Name);
			if (Target != ObjectTable.end())
			{
				if (Target->second->IsRecollectable() &&
					Target->second.use_count() == 1)
				{
					Log::Warn("The searched object({}) is about to be recollected!", _Name.GetNameWithNumber());
				}
				Result = Target->second;
			}
		}
		RWLock.StopReading();

		return Result;
	}

} // namespace VE