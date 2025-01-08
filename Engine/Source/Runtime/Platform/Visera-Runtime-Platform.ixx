module;
#include <Visera.h>

export module Visera.Runtime.Platform;
import :Window;
import Visera.Core.Signal;

VISERA_PUBLIC_MODULE
class ViseraRuntime;

class Platform
{
	friend class ViseraRuntime;
public:
	static inline auto
	GetWindow() -> Window& { return Window::GetInstance(); }

private:
	static inline void
	Tick()
	{
		if  (GetWindow().ShouldClose())
		{ throw EngineStopSignal("Window is being closed..."); }
	}

	static inline void
	Bootstrap()
	{
		GetWindow();
	}

	static inline void
	Terminate()
	{

	}

	Platform() noexcept = default;
};
VISERA_MODULE_END 