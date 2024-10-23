module;
#include <ViseraEngine>

export module Visera.Render.RHI;

import Visera.Render.RHI.Vulkan;

export namespace VE
{

	class RHI
	{
		friend class ViseraRender;
	public:
		

	private:
		RHI() noexcept = default;
		static void
		Bootstrap()
		{
			m_vulkan.Bootstrap();
		}
		static void
		Terminate()
		{
			m_vulkan.Terminate();
		}
		static inline Vulkan m_vulkan;
	};

} // namespace VE