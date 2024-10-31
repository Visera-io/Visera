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
	private:
		static inline UniquePtr<VulkanContext> Vulkan = CreateUniquePtr<VulkanContext>();

	private:
		RHI() noexcept = default;
		static void
		Bootstrap()
		{
			Vulkan->Create();
		}
		static void
		Terminate()
		{
			Vulkan->Destroy();
		}
	};

} // namespace VE