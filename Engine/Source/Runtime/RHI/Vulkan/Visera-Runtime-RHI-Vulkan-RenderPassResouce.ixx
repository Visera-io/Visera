module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassResource;
import :Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderPassResource
	{
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		Bool HasDepthAttachment() const { return DepthAttachment != nullptr; }

	private:
		Array<UniquePtr<FVulkanImage>> ColorAttachments;
		Array<UniquePtr<FVulkanImage>> ResolveAttachments;
		
		UniquePtr<FVulkanImage>		   DepthAttachment;
	};


} } // namespace VE::Runtime