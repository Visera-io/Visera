module;
#include <Visera>

export module Visera.App;
export import Visera.App.Base;
export import Visera.App.Log;
export import Visera.App.Math;
export import Visera.App.Render;

import Visera.Engine;

export namespace VISERA_APP_NAMESPACE
{
	using AppExitSignal = VE::AppExitSignal;

	using Platform		= VE::Runtime::Platform;

	using HiResClock	= VE::HiResClock;
	using SystemClock	= VE::SystemClock;

	class ViseraApp
	{
	public:
		int Run(void(*AppTick)(void)) throw(AppExitSignal)
		{
			try
			{ 
				return VE::Visera::Loop(AppTick);
			}
			catch (const VE::AppExitSignal& Signal)
			{ 
				Log::Info(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
			}
			return EXIT_SUCCESS;
		}

		ViseraApp()
        {
			VE::Visera::Bootstrap();	
			Log::Info("App Started Running");
		}

		~ViseraApp()
		{
			VE::Visera::Terminate();
		}
	};

} // namespace VISERA_APP_NAMESPACE
