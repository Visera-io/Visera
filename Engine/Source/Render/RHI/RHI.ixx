module;
#include <ViseraEngine>

export module Visera.Render.RHI;

import Visera.Render.RHI.Vulkan;

export namespace VE
{
	class ViseraRender;

	class RHI
	{
		friend class ViseraRender;
	public:
		

	private:
		RHI() noexcept = default;
		static void
		Bootstrap()
		{
			m_Vulkan.Bootstrap();
			
			
		}
		static void
		Terminate()
		{
			m_Vulkan.Terminate();
		}
		static inline Vulkan m_Vulkan;
	};

} // namespace VE