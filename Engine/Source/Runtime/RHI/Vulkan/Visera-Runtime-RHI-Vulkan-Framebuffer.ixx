module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Framebuffer;

import :Allocator;
import :Device;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanFramebuffer
	{
		friend class FVulkanRenderPass;
	public:
		Bool HasDepthStencil() const { return DepthStencilImage != nullptr; }

		auto GetHandle()			 { return Handle; }

		FVulkanFramebuffer();
		~FVulkanFramebuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }
		
	private:
		VkFramebuffer		Handle{ VK_NULL_HANDLE };

		Array<SharedPtr<FVulkanImage>> ColorImages;
		Array<SharedPtr<FVulkanImage>> ResolveColorImages; // MSAA Result
		SharedPtr<FVulkanImage>		   DepthStencilImage;
		Array<VkClearValue>			   ClearColors;		

		Array<VkAttachmentDescription>		 ImageDescriptions;
		VkAttachmentDescriptionStencilLayout StencilDescription;
	};

	FVulkanFramebuffer::
	FVulkanFramebuffer()
	{
		// Create Color Images

	}

} } // namespace VE::Runtime