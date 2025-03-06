module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.RTC;
export import  Visera.Runtime.World.RTC.Embree;

import Visera.Core.Signal;

export namespace VE
{
	class World;

	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
		friend class World;
	public:
		using EBuffer	   = EEmbreeBuffer;
		using ECompileType = EEmbreeType;
		using ETopology    = EEmbreeTopology;

		VE_API GetAPI() -> const FEmbree* { return Embree; }

	private:
		static inline FEmbree* Embree = nullptr;
		static inline auto Bootstrap() -> void { Embree = new FEmbree(); }
		static inline auto Terminate() -> void { delete Embree; }
	};

}// namespace VE