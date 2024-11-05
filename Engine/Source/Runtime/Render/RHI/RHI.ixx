module;
#include <ViseraEngine>

export module Visera.Runtime.Render.RHI;

import Visera.Runtime.Render.RHI.Vulkan;

namespace VE
{
	class RenderRuntime;
}

export namespace VE { namespace RHI
{
	
	class Layer
	{
		friend class RenderRuntime;

	private:
		static inline VulkanContext Vulkan{};

	private:
		Layer() noexcept = default;
		static void
		Bootstrap()
		{
			Vulkan.Create();
		}
		static void
		Terminate()
		{
			Vulkan.Destroy();
		}
	};

} } // namespace VE::RHI