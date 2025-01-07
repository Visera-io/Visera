module;
#include <Visera.h>

export module Visera.Runtime.Platform;
export import :Window;

import Visera.Core.Log;

export namespace VE
{
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
			
		}

		static inline void
		Bootstrap()
		{

		}

		static inline void
		Terminate()
		{

		}

		Platform() noexcept = default;
	};
	

VISERA_MODULE_END 