module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassLayout;
import :Common;
import :RenderTargets;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderPassLayout
	{
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
		enum { MaxSimultaneousRenderTargets = 8 };
	public:
		struct FAttachmentDescription
		{
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

		auto AddColorAttachment(FAttachmentDescription _ColorDesc) -> FVulkanRenderPassLayout&;
		auto GetColorAttachmentCount() const -> UInt8 { return ColorDescs.size(); }

		Bool HasDepthImage()				const { return DepthDesc.has_value(); }

		auto GetRenderAreaOffset()			const -> const FVulkanOffset&	{ return RenderAreaOffset; }
		auto GetRenderAreaOffset2D()		const -> const VkOffset2D&		{ return GetRenderAreaOffset().Offset2D; }
		auto GetRenderAreaOffset3D()		const -> const VkOffset3D&		{ return GetRenderAreaOffset().Offset3D; }
		auto GetRenderAreaExtent()			const -> const FVulkanExtent&	{ return RenderAreaExtent; }
		auto GetRenderAreaExtent2D()		const -> const VkExtent2D&		{ return GetRenderAreaExtent().Extent2D; }
		auto GetRenderAreaExtent3D()		const -> const VkExtent3D&		{ return GetRenderAreaExtent().Extent3D; }

		FVulkanRenderPassLayout();
		~FVulkanRenderPassLayout();

	private:
		// (UE5) [[0]ColorImage, [1]ColorImage, ..., [N]ColorImage, [(Auto)N+1]ResolvedImage, ..., [(Auto)N+N]ResolvedImage, [(Auto)Len-1]DepthImage, [(Auto)Len]ShadingRateImage].
		//Segment<FAttachmentDescription, 2 * MaxSimultaneousRenderTargets + 2> AttachmentDescriptions;
		Array<FAttachmentDescription>   ColorDescs;
		Array<FAttachmentDescription>   ResolveDescs;
		Optional<FAttachmentDescription>DepthDesc;
		Optional<FAttachmentDescription>ShadingRateDesc;
		Optional<FStencilDescription>	StencilDesc;
	
		FVulkanOffset					RenderAreaOffset;
		FVulkanExtent					RenderAreaExtent;
		Array<FClearValue>				ClearColors;
	};

	FVulkanRenderPassLayout::
	FVulkanRenderPassLayout()
	{
		DepthDesc = FAttachmentDescription
		{
			.ImageViewType	= EImageViewType::Image2D,
			.LoadOp			= EAttachmentIO::I_Whatever,
			.StoreOp		= EAttachmentIO::O_Store,
			.InitialLayout  = EImageLayout::DepthAttachment,
			.FinalLayout    = EImageLayout::DepthAttachment,
		};
	}

	FVulkanRenderPassLayout::
	~FVulkanRenderPassLayout()
	{

	}

	FVulkanRenderPassLayout& FVulkanRenderPassLayout::
	AddColorAttachment(FAttachmentDescription _ColorDesc)
	{
		ColorDescs.emplace_back(std::move(_ColorDesc));
		ResolveDescs.emplace_back(FAttachmentDescription
			{
				.ImageViewType	= EImageViewType::Image2D,
				.LoadOp			= EAttachmentIO::I_Whatever,
				.StoreOp		= EAttachmentIO::O_Store,
				.InitialLayout  = EImageLayout::Undefined,
				.FinalLayout    = EImageLayout::Present,
			});
		return *this;
	}

} } // namespace VE::Runtime