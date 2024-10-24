module;
#include <ViseraEngine>

export module Visera;
export import Visera.Core;
#ifdef VISERA_RENDER
export import Visera.Render;
#endif

export namespace VE
{

	class Visera
	{
	public:
		static inline void
		Bootstrap()
		{
			Log::Debug("Bootstrapping Visera Core...");
			ViseraCore::Bootstrap();
#ifdef VISERA_RENDER
			Log::Debug("Bootstrapping Visera Render...");
			ViseraRender::Bootstrap();
#endif
		}

		static inline void
		Loop(Bool(*APP)(void))
		{
			static Bool Running = True;
			while (Running)
			{
				// Layers
				if ((Running = APP()) != True) Log::Warn("Visera APP are trying to stop Main Loop.");
			}
		}

		static inline void
		Terminate()
		{
			Log::Debug("Terminating Visera Core...");
			ViseraCore::Terminate();
#ifdef VISERA_RENDER
			Log::Debug("Bootstrapping Visera Render...");
			ViseraRender::Terminate();
#endif
		}
	private:
		Visera() noexcept = default;
	};

} // namespace VE