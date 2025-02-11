module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassLayout;
import :Common;
import :RenderTarget;

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
			EVulkanImageLayout		Setting;
			EVulkanFormat			Format;
			EVulkanSampleRate		SampleRate;
			EVulkanImageViewType	ViewType;
			EVulkanAttachmentIO		LoadOp;
			EVulkanAttachmentIO		StoreOp;
			EVulkanImageLayout		InitialLayout;
			EVulkanImageLayout		FinalLayout;
			EVulkanAttachmentIO		StencilLoadOp	{ EVulkanAttachmentIO::I_Whatever	};
			EVulkanAttachmentIO		StencilStoreOp	{ EVulkanAttachmentIO::O_Whatever };
		};

		struct FStencilDescription
		{
			EVulkanImageLayout	InitialLayout;
			EVulkanImageLayout	FinalLayout;
		};

		auto AddColorAttachment(FAttachmentDescription _ColorDesc) -> FVulkanRenderPassLayout&;
		
		auto GetColorAttachmentCount() const -> UInt8 { return ColorDescs.size(); }
		auto GetTotalAttachmentCount() const -> UInt8 { return ColorDescs.size() + ResolveDescs.size() + (DepthDesc.has_value()? 1 : 0); }
		auto GetDepthAttachmentLocation() const -> UInt8 { VE_ASSERT(HasDepthImage()); return GetTotalAttachmentCount() - 1; }
		auto GetRenderArea()			const -> const FVulkanRenderArea&	{ return RenderArea; }

		Bool HasDepthImage()			const { return DepthDesc.has_value(); }

		FVulkanRenderPassLayout();
		~FVulkanRenderPassLayout();

	private:
		// (UE5) [[0]ColorImage, [1]ColorImage, ..., [N]ColorImage, [(Auto)N+1]ResolvedImage, ..., [(Auto)N+N]ResolvedImage, [(Auto)Len-1]DepthImage, [(Auto)Len]ShadingRateImage].
		Array<FAttachmentDescription>   ColorDescs;
		Array<FAttachmentDescription>   ResolveDescs;
		Optional<FAttachmentDescription>DepthDesc;
		Optional<FAttachmentDescription>ShadingRateDesc;
		Optional<FStencilDescription>	StencilDesc;
	
		FVulkanRenderArea				RenderArea{ 0,0 };
		Array<FClearValue>				ClearColors;
	};

	FVulkanRenderPassLayout::
	FVulkanRenderPassLayout()
	{
		DepthDesc = FAttachmentDescription
		{
			.Setting			= EVulkanImageLayout::Undefined,
			.Format			= EVulkanFormat::S32_Float_Depth32,
			.SampleRate		= EVulkanSampleRate::X1,
			.ViewType		= EVulkanImageViewType::Image2D,
			.LoadOp			= EVulkanAttachmentIO::I_Clear,
			.StoreOp		= EVulkanAttachmentIO::O_Whatever,
			.InitialLayout  = EVulkanImageLayout::Undefined,//[FIXME]
			.FinalLayout    = EVulkanImageLayout::DepthAttachment,
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
				.Setting			= EVulkanImageLayout::Undefined,
				.Format			= ColorDescs.back().Format,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= ColorDescs.back().ViewType,
				.LoadOp			= EVulkanAttachmentIO::I_Whatever,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout  = EVulkanImageLayout::Undefined,
				.FinalLayout    = EVulkanImageLayout::TransferSource,
			});
		return *this;
	}

} } // namespace VE::Runtime