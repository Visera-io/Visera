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
			Vulkan.Bootstrap();
			
			
		}
		static void
		Terminate()
		{
			Vulkan.Terminate();
		}
		static inline VulkanContext Vulkan;
	};

} // namespace VE