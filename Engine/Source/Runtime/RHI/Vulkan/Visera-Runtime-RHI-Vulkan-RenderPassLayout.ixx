module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassLayout;

import :Common;
import :Allocator;

export namespace VE { namespace Runtime
{

	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderPassLayout
	{
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		enum { MaxSimultaneousRenderTargets = 8 };
		struct FAttachmentDescription
		{
			SharedPtr<FVulkanImage> Image;
			EImageViewType  ImageViewType;
			EAttachmentIO	LoadOp;
			EAttachmentIO	StoreOp;
			EImageLayout	InitialLayout;
			EImageLayout	FinalLayout;
			EAttachmentIO	StencilLoadOp	{ EAttachmentIO::I_Whatever	};
			EAttachmentIO	StencilStoreOp	{ EAttachmentIO::O_Whatever };
		};

		struct FStencilDescription
		{
			EImageLayout	InitialLayout;
			EImageLayout	FinalLayout;
		};

		Bool HasDepthImage()				const { return AttachmentCount > ColorImageCount; }

		auto GetRenderAreaOffset()			const -> const FVulkanOffset&	{ return RenderAreaOffset; }
		auto GetRenderAreaOffset2D()		const -> const VkOffset2D&		{ return GetRenderAreaOffset().Offset2D; }
		auto GetRenderAreaOffset3D()		const -> const VkOffset3D&		{ return GetRenderAreaOffset().Offset3D; }
		auto GetRenderAreaExtent()			const -> const FVulkanExtent&	{ return RenderAreaExtent; }
		auto GetRenderAreaExtent2D()		const -> const VkExtent2D&		{ return GetRenderAreaExtent().Extent2D; }
		auto GetRenderAreaExtent3D()		const -> const VkExtent3D&		{ return GetRenderAreaExtent().Extent3D; }

	private:
		// (UE5) [(0)ColorImage, (1)ResolvedImage, ..., (N-1)ColorImage, (N)ResolvedImage, (N+1)DepthImage, (N+2)ShadingRateImage].
		UInt8 AttachmentCount = 0;
		UInt8 ColorImageCount = 0;
		Segment<FAttachmentDescription, 2 * MaxSimultaneousRenderTargets + 2> AttachmentDescriptions;
		Optional<FStencilDescription>	StencilDescription;
		
		//UInt8 ClearValueCount = 0;
		//UInt8 SampleCount     = 0;

		FVulkanOffset RenderAreaOffset;
		FVulkanExtent RenderAreaExtent;
	};

} } // namespace VE::Runtime