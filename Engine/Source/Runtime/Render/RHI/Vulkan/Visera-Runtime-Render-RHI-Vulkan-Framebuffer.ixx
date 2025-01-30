module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Framebuffer;

import :Device;

export namespace VE { namespace Runtime
{

	class FVulkanFramebuffer
	{
	public:
		using TextureIndex = UInt32;
		VkFramebuffer		Handle{ VK_NULL_HANDLE };
		Array<VkImageView>	RenderTargets;
		Array<VkClearValue>	ClearColors;

		operator VkFramebuffer() const { return Handle; }
		~FVulkanFramebuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }
	};

} } // namespace VE::Runtime