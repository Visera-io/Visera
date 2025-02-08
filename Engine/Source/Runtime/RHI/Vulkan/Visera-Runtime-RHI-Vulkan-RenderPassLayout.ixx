module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassLayout;
import :Common;
import :RenderPassResource;

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
			EFormat			Format;
			ESampleRate		SampleRate;
			EImageViewType  ViewType;
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
		auto GetTotalAttachmentCount() const -> UInt8 { return ColorDescs.size() + ResolveDescs.size() + (DepthDesc.has_value()? 1 : 0); }

		Bool HasDepthImage()				const { return DepthDesc.has_value(); }

		auto GetRenderAreaOffset()			const -> const FVulkanOffset3D&	{ return RenderAreaOffset; }
		auto GetRenderAreaExtent()			const -> const FVulkanExtent3D&	{ return RenderAreaExtent; }
		
		FVulkanRenderPassLayout();
		~FVulkanRenderPassLayout();

	private:
		// (UE5) [[0]ColorImage, [1]ColorImage, ..., [N]ColorImage, [(Auto)N+1]ResolvedImage, ..., [(Auto)N+N]ResolvedImage, [(Auto)Len-1]DepthImage, [(Auto)Len]ShadingRateImage].
		Array<FAttachmentDescription>   ColorDescs;
		Array<FAttachmentDescription>   ResolveDescs;
		Optional<FAttachmentDescription>DepthDesc;
		Optional<FAttachmentDescription>ShadingRateDesc;
		Optional<FStencilDescription>	StencilDesc;
	
		FVulkanOffset3D					RenderAreaOffset;
		FVulkanExtent3D					RenderAreaExtent;
		Array<FClearValue>				ClearColors;
	};

	FVulkanRenderPassLayout::
	FVulkanRenderPassLayout()
	{
		DepthDesc = FAttachmentDescription
		{
			.ViewType	= EImageViewType::Image2D,
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
				.ViewType	= EImageViewType::Image2D,
				.LoadOp			= EAttachmentIO::I_Whatever,
				.StoreOp		= EAttachmentIO::O_Store,
				.InitialLayout  = EImageLayout::Undefined,
				.FinalLayout    = EImageLayout::Present,
			});
		return *this;
	}

} } // namespace VE::Runtime