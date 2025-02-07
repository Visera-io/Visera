module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderTargets;
import :Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderTargets
	{
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		Bool HasDepthImage() const { return DepthImage != nullptr; }

	private:
		Array<SharedPtr<FVulkanImage>> ColorImages;
		Array<SharedPtr<FVulkanImage>> ResolveImages;
		
		SharedPtr<FVulkanImage>		   DepthImage;
	};


} } // namespace VE::Runtime