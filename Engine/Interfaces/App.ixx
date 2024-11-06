module;
#include <ViseraEngine>
export module Visera.App;
export import Visera.App.Base;
export import Visera.App.Log;

import Visera;

export namespace VISERA_APP_NAMESPACE
{

	class ViseraApp
	{
	public:
		void
		Tick() throw(VE::AppExitSignal)
		{
			//YOUR CODES
			
			throw VE::AppExitSignal("Main Tick");
		}

		ViseraApp()
        {
			VE::Visera::Bootstrap();	
			/*{
				VE::HiResClock Clock{};
				VISERA_APP_NAMESPACE::Log::Info("App Started Running");
				VE::Visera::Loop(Minimal::App::Tick);
				VISERA_APP_NAMESPACE::Log::Info("App Running Time: {}ms", Clock.GetTotalTime().milliseconds());
			}*/
		}

		~ViseraApp()
		{
			VE::Visera::Terminate();
		}
	};

} // namespace VISERA_APP_NAMESPACE
