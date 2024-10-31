module;
#include <ViseraEngine>

export module Visera;
export import Visera.Core;
#if defined(VISERA_PLATFORM)
export import Visera.Platform;
#endif
#if defined(VISERA_RENDER)
export import Visera.Render;
#endif

import Visera.Internal;

export namespace VE
{

	class Visera
	{
	public:
		static inline void
		Bootstrap()
		{
			Log::Debug("Bootstrapping Visera Internal...");
			ViseraInternal::Bootstrap();
			Log::Debug("Bootstrapping Visera Core...");
			ViseraCore::Bootstrap();
#if defined(VISERA_PLATFORM) || defined(VISERA_RENDER)
			Log::Debug("Bootstrapping Visera Platform...");
			ViseraPlatform::Bootstrap();
#endif
#if defined(VISERA_RENDER)
			Log::Debug("Bootstrapping Visera Render...");
			ViseraRender::Bootstrap();
#endif
		}

		static inline void
		Loop(Bool(*AppTick)(void))
		{
			try { do {
				ViseraInternal::Tick();
				ViseraCore::Tick();
				if (AppTick() != EXIT_SUCCESS)
				{ ViseraInternal::Context.MainLoop.Stop(VISERA_APP_NAME); }
#if defined(VISERA_PLATFORM)
				ViseraPlatform::Tick();
#endif
#if defined(VISERA_RENDER)
				ViseraRender::Tick();
#endif
			} while (!ViseraInternal::Context.MainLoop.ShouldStop()); }
			catch (const VE::RuntimeError& Error)
			{
				return Log::Error("Unsolved Visera runtime error:\n{}{}", Error.What(), Error.Where());
			}
		}

		static inline void
		Terminate()
		{
#if defined(VISERA_RENDER)
			Log::Debug("Terminating Visera Render...");
			ViseraRender::Terminate();
#endif
#if defined(VISERA_PLATFORM) || defined(VISERA_RENDER)
			Log::Debug("Terminating Visera Platform...");
			ViseraPlatform::Terminate();
#endif
			Log::Debug("Terminating Visera Core...");
			ViseraCore::Terminate();
			Log::Debug("Terminating Visera Internal...");
			ViseraInternal::Terminate();
		}
	private:
		Visera() noexcept = default;
	};

} // namespace VE