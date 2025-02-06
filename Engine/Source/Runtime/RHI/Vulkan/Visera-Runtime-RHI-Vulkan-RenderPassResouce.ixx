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
		struct FColorAttachment
		{
			UniquePtr<FVulkanImage>	Image;
			UniquePtr<FVulkanImage>	ResolveImage; // MSAA Result
		};
		auto GetColorAttachment(UInt8 _Binding) const -> const FColorAttachment& { VE_ASSERT(_Binding < ColorAttachments.size()); return ColorAttachments[_Binding]; }

		Bool HasDepthAttachment() const { return DepthAttachment != nullptr; }

	private:
		Array<FColorAttachment>		ColorAttachments;
		UniquePtr<FVulkanImage>		DepthAttachment;
	};


} } // namespace VE::Runtime