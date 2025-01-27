module;
#include <Visera.h>
#if defined(VE_ON_WINDOWS_SYSTEM)
#include <Windows.h>
#endif
export module Visera.Core;
export import Visera.Core.Type;
export import Visera.Core.Log;
export import Visera.Core.System;
export import Visera.Core.Signal;
export import Visera.Core.Time;
export import Visera.Core.Math;
export import Visera.Core.Media;

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
#if defined(VE_ON_WINDOWS_SYSTEM)
		SetConsoleOutputCP(65001); // Enable Terminal WideString Output
#endif
	}

	void ViseraCore::
	Terminate()
	{

	}

} // namespace VE