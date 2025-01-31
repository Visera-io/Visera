module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Framebuffer;

import :Device;

export namespace VE { namespace Runtime
{

	class FVulkanFramebuffer
	{
	public:
		using TextureIndex = UInt32;
		
		auto GetHandle()			{ return Handle; }
		operator VkFramebuffer()	{ return Handle; }
		FVulkanFramebuffer() = default;
		~FVulkanFramebuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }
		
	private:
		VkFramebuffer		Handle{ VK_NULL_HANDLE };
		Array<VkImageView>	RenderTargets;
		Array<VkClearValue>	ClearColors;
	};

} } // namespace VE::Runtime