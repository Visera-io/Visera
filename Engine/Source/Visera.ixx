module;
#include <Visera.h>

export module Visera;
export import Visera.Core;
#if defined(VISERA_RUNTIME)
export import Visera.Runtime;
#endif


VISERA_PUBLIC_MODULE
class ViseraApp
{
public:
	virtual void Bootstrap() = 0;
	virtual void Terminate() = 0;
	virtual void Tick() = 0;
	virtual void RenderTick() = 0;

	void inline
	Exit(const AppExitSignal& Message = AppExitSignal("Visera App Exited Successfully.")) const throw(AppExitSignal) { throw Message; }

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
				Log::Info("App Started Running");
				do { App->Tick(); } while (RuntimeTick());
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
	}
private:
	ViseraApp* const App = nullptr;
	static inline std::function<Bool()> RuntimeTick = []() -> Bool { return False; };
};

VISERA_MODULE_END