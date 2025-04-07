module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPass;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Allocator;
import :Device;
import :Swapchain;
import :Shader;
import :PipelineCache;
import :Framebuffer;
import :RenderPipeline;
import :RenderPassLayout;
import :RenderTarget;

import Visera.Core.Signal;

export namespace VE
{

	class FVulkanRenderPass
	{
	public:
		enum class EType { Background, DefaultForward, Postprocessing, Overlay, Customized };
		struct FSubpass final
		{
			SharedPtr<FVulkanRenderPipeline>Pipeline;

			Array<UInt8>					ColorImageReferences;
			//Optional<VkAttachmentReference> StencilImageReference;
			EVulkanGraphicsPipelineStage	SrcStage				{ EVulkanGraphicsPipelineStage::None };
			EVulkanAccess					SrcStageAccess			{ EVulkanAccess::None };
			EVulkanGraphicsPipelineStage	DstStage				{ EVulkanGraphicsPipelineStage::None };
			EVulkanAccess					DstStageAccess			{ EVulkanAccess::None };

			Array<UInt8>					InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt8>					PreserveImageReferences; // Const Image References Used in Subpasses.

			Bool							bEnableDepthTest		= True;
			Bool							bExternalSubpass		= False; // Load from external libs (e.g., ImGUI)
		};

		auto GetType()				  const -> EType { return Type; }
		auto GetRenderArea()		  const -> const FVulkanRenderArea& { return RenderArea; }
		auto GetSubpasses()			  const -> const Array<FSubpass>& { return Subpasses; }
		auto GetRenderPassBeginInfo() const -> const VkRenderPassBeginInfo;
		auto GetHandle()			  const	-> const VkRenderPass { return Handle; }

		void AddSubpass(const FSubpass& _SubpassInfo);
		Bool HasSubpass(SharedPtr<const FVulkanRenderPipeline> _SubpassPipeline) const;

		void Build(const FVulkanRenderArea& _RenderArea, const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets);
		void Destroy();
	
	protected:
		EType							Type;
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		FVulkanRenderArea				RenderArea{ 0,0 };
		FVulkanRenderPassLayout			Layout;
		Array<FClearValue>				ClearValues;
		Array<FVulkanFramebuffer>		Framebuffers;
		Array<FSubpass>					Subpasses;

	public:
		FVulkanRenderPass() = delete;
		FVulkanRenderPass(EType _RenderPassType);
		~FVulkanRenderPass() noexcept { Destroy(); }
	};

	FVulkanRenderPass::
	FVulkanRenderPass(EType _RenderPassType)
		:Type{_RenderPassType}
	{
		switch (Type)
		{
		case EType::Background:
		{
			static Bool bCreated = False;
			if (bCreated) { throw SRuntimeError("You can only create one Background Pass!"); }
			Layout.AddColorAttachmentDesc(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Clear,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ShaderReadOnly, //At the start of each frame, the ColorImage is sampled by the Editor.
				.FinalLayout	= EVulkanImageLayout::ShaderReadOnly,
			});
			Layout.DepthDesc =
			{
				.Layout			= EVulkanImageLayout::DepthStencilAttachment,
				.Format			= EVulkanFormat::S32_Float_Depth32,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Clear,
				.StoreOp		= EVulkanAttachmentIO::O_Whatever,
				.InitialLayout  = EVulkanImageLayout::DepthStencilAttachment,
				.FinalLayout    = EVulkanImageLayout::DepthStencilAttachment,
			};
			bCreated = True;
			break;
		}
		case EType::DefaultForward:
		{
			Layout.AddColorAttachmentDesc(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Keep,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ShaderReadOnly,
				.FinalLayout	= EVulkanImageLayout::ShaderReadOnly,
			});
			Layout.DepthDesc =
			{
				.Layout			= EVulkanImageLayout::DepthStencilAttachment,
				.Format			= EVulkanFormat::S32_Float_Depth32,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Clear,
				.StoreOp		= EVulkanAttachmentIO::O_Whatever,
				.InitialLayout  = EVulkanImageLayout::DepthStencilAttachment,
				.FinalLayout    = EVulkanImageLayout::DepthStencilAttachment,
			};
			break;
		}
		case EType::Postprocessing:
		{
			Layout.AddColorAttachmentDesc(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Keep,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ShaderReadOnly,
				.FinalLayout	= EVulkanImageLayout::ShaderReadOnly,
			});
			break;
		}
		case EType::Overlay:
		{
			Layout.AddColorAttachmentDesc(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= GVulkan->Swapchain->GetFormat(),
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Clear,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::Present,
				.FinalLayout	= EVulkanImageLayout::Present,
			});
			break;
		}
		case EType::Customized:
		{
			break;
		}
		default: throw SRuntimeError("Unkonwn RenderPassLayout Preset!");
		}
	}

	void FVulkanRenderPass::
	Build(const FVulkanRenderArea& _RenderArea,
	      const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets)
	{
		VE_ASSERT(_RenderArea.extent.width > 0 && _RenderArea.extent.height > 0);
		VE_ASSERT(!Subpasses.empty());

		// Create RenderPass
		Array<VkSubpassDescription> SubpassDescriptions(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencies(Subpasses.size());
		Array<Array<VkAttachmentReference>> ColorRefTable(Subpasses.size());
		Array<Array<VkAttachmentReference>> ResolveRefTable(Subpasses.size());
		Array<VkAttachmentReference>		DepthRefTable(Subpasses.size());
		
		ClearValues.reserve(Layout.GetTotalAttachmentCount());

		for (UInt32 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& CurrentSubpass = Subpasses[Idx];

			auto& ColorRefs = ColorRefTable[Idx];
			ColorRefs.resize(CurrentSubpass.ColorImageReferences.size());
			for (UInt8 Idx = 0; Idx < ColorRefs.size(); ++Idx)
			{
				ColorRefs[Idx].attachment	= CurrentSubpass.ColorImageReferences[Idx];
				ColorRefs[Idx].layout		= AutoCast(Layout.ColorDescs[Idx].Layout);
				ClearValues.emplace_back(FClearValue{ .color{1.0f, 0.0f, 1.0f, 1.0f} });
			}

			auto& ResolveRefs = ResolveRefTable[Idx];
			if (Layout.HasResolveImage())
			{
				VE_WIP;// MSAA WIP
				ResolveRefs.resize(ColorRefs.size());
				for (UInt8 Idx = 0; Idx < ColorRefs.size(); ++Idx)
				{
					ResolveRefs[Idx].attachment = Layout.GetColorAttachmentCount() + CurrentSubpass.ColorImageReferences[Idx];
					ResolveRefs[Idx].layout		= AutoCast(Layout.ResolveDescs[Idx].Layout);
					ClearValues.emplace_back(FClearValue{ .color{1.0f, 0.0f, 0.0f, 1.0f} });
				}
			}

			auto& DepthRef = DepthRefTable[Idx];
			if (Layout.HasDepthImage())
			{
				DepthRef = VkAttachmentReference
				{
					.attachment = Layout.GetDepthAttachmentLocation(),
					.layout		= AutoCast(Layout.DepthDesc.value().Layout),
				};
				ClearValues.emplace_back(FClearValue{ .depthStencil{.depth = 1.0f, .stencil = 0} });
			};
			
			SubpassDescriptions[Idx] =
			{
				.flags					= 0x0,
				.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount	= 0, //[FIXME]
				.pInputAttachments		= nullptr,
				.colorAttachmentCount	= UInt32(ColorRefs.size()),
				.pColorAttachments		= ColorRefs.data(),
				.pResolveAttachments	= ResolveRefs.empty()? nullptr : ResolveRefs.data(),
				.pDepthStencilAttachment= Layout.HasResolveImage()? &DepthRef : nullptr,
				.preserveAttachmentCount= 0,
				.pPreserveAttachments	= nullptr,
			};

			static_assert(VK_SUBPASS_EXTERNAL == (0U - 1));
			SubpassDependencies[Idx] =
			{
				.srcSubpass		= Idx - 1,
				.dstSubpass		= Idx,
				.srcStageMask	= AutoCast(CurrentSubpass.SrcStage),
				.dstStageMask	= AutoCast(CurrentSubpass.DstStage),
				.srcAccessMask	= AutoCast(CurrentSubpass.SrcStageAccess),
				.dstAccessMask	= AutoCast(CurrentSubpass.DstStageAccess),
				.dependencyFlags= 0x0,
			};
		}
		
		auto AttachmentDescriptions = Array<VkAttachmentDescription>(Layout.GetTotalAttachmentCount());

		for (UInt8 Idx = 0; Idx < Layout.GetColorAttachmentCount(); ++Idx)
		{
			auto& ColorDesc    = Layout.ColorDescs[Idx];
			// Color Attachments
			AttachmentDescriptions[Idx] = VkAttachmentDescription
			{ 
				.flags			= 0x0,
				.format			= AutoCast(ColorDesc.Format),
				.samples		= AutoCast(ColorDesc.SampleRate),
				.loadOp			= AutoCast(ColorDesc.LoadOp),
				.storeOp		= AutoCast(AutoCast(ColorDesc.StoreOp)),
				.stencilLoadOp	= AutoCast(ColorDesc.StencilLoadOp),
				.stencilStoreOp = AutoCast(AutoCast(ColorDesc.StencilStoreOp)),
				.initialLayout	= AutoCast(ColorDesc.InitialLayout),
				.finalLayout	= AutoCast(ColorDesc.FinalLayout),
			};
			// Resolve Attachments
			if (Layout.HasResolveImage())
			{
				auto& ResolveDesc   = Layout.ResolveDescs[Idx];
				AttachmentDescriptions[Layout.GetColorAttachmentCount() + Idx] = VkAttachmentDescription
				{ 
					.flags			= 0x0,
					.format			= AutoCast(ResolveDesc.Format),
					.samples		= AutoCast(ResolveDesc.SampleRate),
					.loadOp			= AutoCast(ResolveDesc.LoadOp),
					.storeOp		= AutoCast(AutoCast(ResolveDesc.StoreOp)),
					.stencilLoadOp	= AutoCast(ResolveDesc.StencilLoadOp),
					.stencilStoreOp = AutoCast(AutoCast(ResolveDesc.StencilStoreOp)),
					.initialLayout	= AutoCast(ResolveDesc.InitialLayout),
					.finalLayout	= AutoCast(ResolveDesc.FinalLayout),
				};
			}
		}
		// Depth Attachment
		if (Layout.HasDepthImage())
		{
			auto& DepthDesc   = Layout.DepthDesc.value();
			AttachmentDescriptions.back() = VkAttachmentDescription
			{
				.flags			= 0x0,
				.format			= AutoCast(DepthDesc.Format),
				.samples		= AutoCast(DepthDesc.SampleRate),
				.loadOp			= AutoCast(DepthDesc.LoadOp),
				.storeOp		= AutoCast(AutoCast(DepthDesc.StoreOp)),
				.stencilLoadOp	= AutoCast(DepthDesc.StencilLoadOp),
				.stencilStoreOp = AutoCast(AutoCast(DepthDesc.StencilStoreOp)),
				.initialLayout	= AutoCast(DepthDesc.InitialLayout),
				.finalLayout	= AutoCast(DepthDesc.FinalLayout),
			};
		}

		VkRenderPassCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = UInt32(AttachmentDescriptions.size()),
			.pAttachments	 = AttachmentDescriptions.data(),
			.subpassCount	 = UInt32(Subpasses.size()),
			.pSubpasses		 = SubpassDescriptions.data(),
			.dependencyCount = UInt32(Subpasses.size()),
			.pDependencies	 = SubpassDependencies.data(),
		};
		if(vkCreateRenderPass(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan RenderPass!"); }

		// Create Subpasses (Setted in the derived class)
		for (UInt8 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& CurrentSubpass = Subpasses[Idx];

			if (CurrentSubpass.Pipeline)
			{			
				CurrentSubpass.Pipeline->Build(Handle, Idx);
			}
			else if (CurrentSubpass.bExternalSubpass)
			{
				
			}
			else { throw SRuntimeError("Cannot Build a Subpass without any Pipeline!"); }
		}

		// Create Framebuffers
		RenderArea = _RenderArea;
		FVulkanExtent3D Extent{ RenderArea.extent.width, RenderArea.extent.height, 1 };

		//[TODO]: Redesign this API
		if (Type == EType::Postprocessing)
		{
			VE_ASSERT(_RenderTargets.size() == GVulkan->Swapchain->GetFrameCount());
			Framebuffers.resize(_RenderTargets.size());

			for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
			{
				Framebuffers[Idx].Build(Handle, Extent, _RenderTargets[Idx], True);
			}
		}
		else if (Type != EType::Overlay)
		{
			VE_ASSERT(_RenderTargets.size() == GVulkan->Swapchain->GetFrameCount());
			Framebuffers.resize(_RenderTargets.size());

			for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
			{
				Framebuffers[Idx].Build(Handle, Extent, _RenderTargets[Idx]);
			}
		}
		else
		{
			ClearValues[0] = FClearValue{ .color{0.0f, 0.0f, 0.0f, 1.0f} };
			const auto& SwapchainImages = GVulkan->Swapchain->GetImages();
			const auto SwapchainFormat = AutoCast(GVulkan->Swapchain->GetFormat());
			// Create Swapchain Image Views
			Framebuffers.resize(SwapchainImages.size());
			for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
			{
				auto& CurrentFramebuffer = Framebuffers[Idx];
				CurrentFramebuffer.RenderTargetViews.resize(1);
				VkImageViewCreateInfo ImageViewCreateInfo
				{
					.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image		= SwapchainImages[Idx],
					.viewType	= AutoCast(EVulkanImageViewType::Image2D),
					.format		= SwapchainFormat,
					.components{
						.r = AutoCast(EVulkanSwizzle::Identity),
						.g = AutoCast(EVulkanSwizzle::Identity),
						.b = AutoCast(EVulkanSwizzle::Identity),
						.a = AutoCast(EVulkanSwizzle::Identity)
						},
					.subresourceRange{
						.aspectMask		= AutoCast(EVulkanImageAspect::Color),
						.baseMipLevel	= 0,
						.levelCount		= 1,
						.baseArrayLayer = 0,
						.layerCount		= 1
						}
				};
				if(vkCreateImageView(
					GVulkan->Device->GetHandle(),
					&ImageViewCreateInfo,
					GVulkan->AllocationCallbacks,
					&CurrentFramebuffer.RenderTargetViews[0]) != VK_SUCCESS)
				{ throw SRuntimeError("Failed to create Vulkan Image View!"); }

				VkFramebufferCreateInfo FramebufferCreateInfo
				{
					.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.pNext			 = nullptr,
					.flags			 = 0x0,
					.renderPass		 = Handle,
					.attachmentCount = UInt32(CurrentFramebuffer.RenderTargetViews.size()),
					.pAttachments	 = CurrentFramebuffer.RenderTargetViews.data(),
					.width  = Extent.width,
					.height = Extent.height,
					.layers = Extent.depth,
				};

				if (vkCreateFramebuffer(
					GVulkan->Device->GetHandle(),
					&FramebufferCreateInfo,
					GVulkan->AllocationCallbacks,
					&CurrentFramebuffer.Handle) != VK_SUCCESS)
				{ throw SRuntimeError("Failed to create Vulkan Framebuffer for current Overlay Pass!"); }
			}
		}
	}

	void FVulkanRenderPass::
	Destroy()
	{
		if (Handle != VK_NULL_HANDLE)
		{	
			for (auto& Framebuffer : Framebuffers)
			{ Framebuffer.Destroy(); }

			for (auto& Subpass : Subpasses)
			{ if (Subpass.Pipeline) Subpass.Pipeline->Destroy(); }

			vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}
	const VkRenderPassBeginInfo FVulkanRenderPass::
	GetRenderPassBeginInfo() const
	{
		return VkRenderPassBeginInfo
		{
			.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext			= nullptr,
			.renderPass		= Handle,
			.framebuffer	= Framebuffers[GVulkan->Swapchain->GetCursor()].Handle,
			.renderArea		= RenderArea,
			.clearValueCount= UInt32(ClearValues.size()),
			.pClearValues	= ClearValues.data()
		};
	}

	void FVulkanRenderPass::
	AddSubpass(const FSubpass& _SubpassInfo)
	{
		if(!_SubpassInfo.Pipeline && !_SubpassInfo.bEnableDepthTest)
		{ throw SRuntimeError("Cannot create a subpass without a pipeline!"); }
		
		if(_SubpassInfo.ColorImageReferences.empty())
		{ throw SRuntimeError("Cannot create a subpass without any color reference!"); }

		if(_SubpassInfo.SrcStage == EVulkanGraphicsPipelineStage::None ||
		   _SubpassInfo.DstStage == EVulkanGraphicsPipelineStage::None)
		{ throw SRuntimeError("Cannot create a subpass without assigning src/dst stages!"); }

		Subpasses.emplace_back(std::move(_SubpassInfo));
	}

	Bool FVulkanRenderPass::
	HasSubpass(SharedPtr<const FVulkanRenderPipeline> _SubpassPipeline) const
	{
		for (const auto& Subpass : Subpasses)
		{
			if (_SubpassPipeline->GetHandle() == Subpass.Pipeline->GetHandle())
			{
				return True;
			}
		}
		return False;
	}

} // namespace VE