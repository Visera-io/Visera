module;
#include <Visera.h>
export module Visera.Runtime.World;
export import Visera.Runtime.World.RTC;
export import Visera.Runtime.World.Ray;
export import Visera.Runtime.World.Geometry;
import Visera.Runtime.World.Atlas;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Stage;

import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.Math;
import Visera.Core.System.Concurrency;
import Visera.Core.System.FileSystem;

export namespace VE
{
	class World
	{
		VE_MODULE_MANAGER_CLASS(World);
	public:
		VE_API GetCoordinate() -> const FViseraChart& { return Atlas::Visera; }
		VE_API CreateCoordinateSystem(const Vector3F& _Pivot) -> ResultPackage<Vector3F, Vector3F, Vector3F>;

		using VObject = VObject;
		template<VObjectType T> static inline 
		SharedPtr<T> CreateObject(FName _Name);
		VE_API SearchObject(FName _Name)->SharedPtr<VObject>;
		
		VE_API CreateStage(FName _StageName, const String& _StageFile) -> SharedPtr<FStage> { return CreateSharedPtr<FStage>(_StageName, _StageFile); }

	private:
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
		RTC::Bootstrap();
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

		RTC::Terminate();
	}

	ResultPackage<Vector3F, Vector3F, Vector3F> World::
	CreateCoordinateSystem(const Vector3F& _Pivot)
	{
		Float Sign = std::copysign(1.0f, _Pivot.z());
		Float Coef_a = -1.0 / (Sign + _Pivot.z());
		Float Coef_b = _Pivot.x() * _Pivot.y() * Coef_a;
		Vector3F Va {(1 + Sign * (_Pivot.x() * _Pivot.x()) * Coef_a),
					 (Sign * Coef_b),
					 (-Sign * _Pivot.x())};
		Vector3F Vb {(Coef_b),
					 (Sign + (_Pivot.y() * _Pivot.y()) * Coef_a),
					 (-_Pivot.y())};
		return {_Pivot, Va, Vb};
	}

	template<VObjectType T>
	SharedPtr<T> World::
	CreateObject(FName _Name)
	{ 
		static_assert(VObjectType<T>, "T must satisfy the VObjectType concept!");
		SharedPtr<T> Result = nullptr;

		RWLock.StartWriting();
		{
			auto& ObjectSlot = ObjectTable[_Name];
			if (ObjectSlot == nullptr)
			{
				auto NewObject = CreateSharedPtr<T>();
				NewObject->Name = _Name;
				NewObject->Create();
				ObjectSlot = NewObject;
				Result = NewObject;
			}
			else
			{
				Log::Warn("You may create a duplicated Object({}) to the World."
						  "-- If you want to do that, try to increase the number of FName.",
						  _Name.GetNameWithNumber()); 
			}
		}
		RWLock.StopWriting();

		return Result;
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
				if (Target->second->bRecollectable &&
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