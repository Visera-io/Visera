module;
#include <Visera.h>

export module Visera.Engine;
export import Visera.Core;
#if defined(VISERA_RUNTIME)
export import Visera.Runtime;
#endif
import Visera.App;
import Visera.Internal;

namespace VE
{

	export class Visera final
	{
	public:
		Int32 inline
		Run()
		{
			try
			{
				if (App)
				{
					Log::Info("App Started Running");
					do { App->Tick(); } while (RuntimeTick(App));
				}
				else Log::Warn("Visera App is not created");
			}
			catch (const VE::AppExitSignal& Signal)
			{
				Log::Info(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
			}
			catch (const VE::RuntimeError& Error)
			{
				Log::Error("Unsolved Visera runtime error:\n{}{}", Error.What(), Error.Where());
				return EXIT_FAILURE;
			}
			catch (const std::exception& Error)
			{ 
				Log::Error("Unexcepted Error:\n{}", Error.what());
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}

		Visera(ViseraApp* App) : App{ App }
		{
			Log::Debug("Bootstrapping Visera Internal...");
			ViseraInternal::Bootstrap();
			Log::Debug("Bootstrapping Visera Core...");
			ViseraCore::Bootstrap();
#if defined(VISERA_RUNTIME)
			Log::Debug("Bootstrapping Visera Runtime...");
			ViseraRuntime::Bootstrap();
			RuntimeTick = ViseraRuntime::Tick;
#endif
			if (App) App->Bootstrap();
		}

		~Visera()
		{
			if (App) { App->Terminate(); delete App; }
#if defined(VISERA_RUNTIME)
			Log::Debug("Terminating Visera Runtime...");
			ViseraRuntime::Terminate();
#endif
			Log::Debug("Terminating Visera Core...");
			ViseraCore::Terminate();
			Log::Debug("Terminating Visera Internal...");
			ViseraInternal::Terminate();
		}
	private:
		ViseraApp* const App = nullptr;
		static inline std::function<Bool(ViseraApp* App)> RuntimeTick = [](ViseraApp* App) -> Bool { return False; };
	};

} // namespace VE