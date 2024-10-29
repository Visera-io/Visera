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
			Vulkan = CreateUniquePtr<VulkanContext>();
		}
		static void
		Terminate()
		{
			Vulkan.reset();
		}
		static inline UniquePtr<VulkanContext> Vulkan;
	};

} // namespace VE