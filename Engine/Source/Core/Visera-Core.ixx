module;
#include <Visera.h>
export module Visera.Core;
export import Visera.Core.Type;
export import Visera.Core.Log;
export import Visera.Core.System;
export import Visera.Core.Signal;
export import Visera.Core.Time;
export import Visera.Core.Math;

export namespace VE
{
	class Visera;

	class ViseraCore
	{
		VE_MODULE_MANAGER_CLASS(ViseraCore);
		VE_API Bootstrap() -> void;
		VE_API Terminate() -> void;
	};

	void ViseraCore::
	Bootstrap()
	{
		//Log::SystemLogger = CreateUniquePtr<FSystemLogger>();
		//VISERA_APP_NAMESPACE::Log::AppLogger = CreateUniquePtr<FAppLogger>();
	}

	void ViseraCore::
	Terminate()
	{
		//VISERA_APP_NAMESPACE::Log::AppLogger.reset();
		//Log::SystemLogger.reset();
	}

} // namespace VE