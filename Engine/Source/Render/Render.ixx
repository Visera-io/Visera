module;
#include <ViseraEngine>

export module Visera.Render;
export import Visera.Render.RHI;
export import Visera.Render.Coordinate;

export namespace VE
{
	class Visera;

	class ViseraRender
	{
		friend class Visera;
	private:
		static inline void
		Bootstrap()
		{
			RHI::Bootstrap();
		}

		static inline void
		Terminate()
		{
			RHI::Terminate();
		}

		ViseraRender() noexcept = default;
	};
	

} // namespace VE