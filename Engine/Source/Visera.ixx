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
	using RTC		= VE::Runtime::RTC;
	using Window	= VE::Runtime::Window;
	using World		= VE::Runtime::World;
	using Geometry  = VE::Runtime::Geometry;

	using FRay		= VE::Runtime::FRay;


	class ViseraApp
	{
	public:
		virtual void Bootstrap() = 0;
		virtual void Terminate() = 0;
		virtual void Tick() = 0;
		virtual void RenderTick() = 0;

		void inline
		Exit(const SAppStop& Message = SAppStop("Visera App Exited Successfully.")) const throw(SAppStop) { throw Message; }

		ViseraApp()	 noexcept = default;
		~ViseraApp() noexcept = default;
	};

	class Visera final
	{
	public:
		Int32 inline
		Run()
		{
			Int32 StateCode = EXIT_SUCCESS;
			try
			{
				if (App)
				{
					Log::Debug("Bootstrapping the " VISERA_APP_NAME "...");
					App->Bootstrap();
					{
						do { App->Tick(); } while (RuntimeTick());
					}
					App->Terminate();
					delete App;
				}
				else Log::Error("Visera App is not created");
			}
			catch (const SAppStop& Signal)
			{
				Log::Debug(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			catch (const SEngineStop& Signal)
			{
				Log::Debug("Visera Engine Stopped:\n{}{}", Signal.What(), Signal.Where());
				StateCode = Signal.StateCode;
			}
			return StateCode;
		}

		Visera(ViseraApp* App) : App{ App }
		{
			ViseraCore::Bootstrap();
	#if defined(VISERA_RUNTIME)
			Log::Debug("Bootstrapping Visera Runtime...");
			Runtime::ViseraRuntime::Bootstrap();
			RuntimeTick = Runtime::ViseraRuntime::Tick;
	#endif

		}

		~Visera()
		{
	#if defined(VISERA_RUNTIME)
			Log::Debug("Terminating Visera Runtime...");
			Runtime::ViseraRuntime::Terminate();
	#endif
			ViseraCore::Terminate();
		}
	private:
		ViseraApp* const App = nullptr;
		static inline std::function<Bool()> RuntimeTick = []() -> Bool { return False; };
	};

} // namespace VE