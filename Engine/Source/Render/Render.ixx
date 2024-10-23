module;
#include <ViseraEngine>

export module Visera.Render;
export import Visera.Render.RHI;

export namespace VE
{
	
	class ViseraRender
	{
		friend class Visera;
	private:
		static inline void
		Bootstrap()
		{
			
		}

		static inline void
		Terminate()
		{
			
		}

		ViseraRender() noexcept = default;
	};
	

} // namespace VE