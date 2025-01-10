module;
#include <Visera.h>

export module Visera;
export import Visera.Core;
#if defined(VISERA_RUNTIME)
import Visera.Runtime;
#endif


export namespace VE
{
	using RHI		= VE::Runtime::RHI;
	using UI		= VE::Runtime::UI;
	using Window	= VE::Runtime::Window;

	class ViseraApp
	{
	public:
		virtual void Bootstrap() = 0;
		virtual void Terminate() = 0;
		virtual void Tick() = 0;
		virtual void RenderTick() = 0;

		void inline
		Exit(const AppStopSignal& Message = AppStopSignal("Visera App Exited Successfully.")) const throw(AppStopSignal) { throw Message; }

		ViseraApp()	 noexcept = default;
		~ViseraApp() noexcept = default;
	};

	class Visera final
	{
	public:
		Int32 inline
		Run()
		{
			try
			{
				if (App)
				{
					Log::Debug("App Started Running");
					do { App->Tick(); } while (RuntimeTick());
				}
				else Log::Error("Visera App is not created");
			}
			catch (const AppStopSignal& Signal)
			{
				Log::Debug(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
				return Signal.StateCode;
			}
			catch (const EngineStopSignal& Signal)
			{
				Log::Debug("Visera Engine Stopped:\n{}{}", Signal.What(), Signal.Where());
				return Signal.StateCode;
			}
			return EXIT_SUCCESS;
		}

		Visera(ViseraApp* App) : App{ App }
		{
			Log::Debug("Bootstrapping Visera Core...");
			ViseraCore::Bootstrap();
	#if defined(VISERA_RUNTIME)
			Log::Debug("Bootstrapping Visera Runtime...");
			Runtime::Bootstrap();
			RuntimeTick = Runtime::Tick;
	#endif
			if (App) App->Bootstrap();
		}

		~Visera()
		{
			if (App) { App->Terminate(); delete App; }
	#if defined(VISERA_RUNTIME)
			Log::Debug("Terminating Visera Runtime...");
			Runtime::Terminate();
	#endif
			Log::Debug("Terminating Visera Core...");
			ViseraCore::Terminate();
		}
	private:
		ViseraApp* const App = nullptr;
		static inline std::function<Bool()> RuntimeTick = []() -> Bool { return False; };
	};

} // namespace VE