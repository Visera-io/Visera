module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPass;
import :Common;
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
	class RHI;

	class FVulkanRenderPass
	{
		friend class RHI;
	public:
		enum class ERenderPassType { Background, DefaultForward, Overlay, Customized };
		struct FSubpass final
		{
			SharedPtr<FVulkanRenderPipeline>Pipeline;
			WeakPtr<const FVulkanShader>	VertexShader;
			WeakPtr<const FVulkanShader>	FragmentShader;

			Array<UInt8>					ColorImageReferences;
			//Optional<VkAttachmentReference> StencilImageReference;
			EVulkanGraphicsPipelineStage	SrcStage				{ EVulkanGraphicsPipelineStage::PipelineTop };
			EVulkanAccess					SrcStageAccess			{ EVulkanAccess::None };
			EVulkanGraphicsPipelineStage	DstStage				{ EVulkanGraphicsPipelineStage::None };
			EVulkanAccess					DstStageAccess			{ EVulkanAccess::None };

			Array<UInt8>					InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt8>					PreserveImageReferences; // Const Image References Used in Subpasses.

			Bool							bEnableDepthTest		= True;
			Bool							bExternalSubpass		= False; // Load from external libs (e.g., ImGUI)
		};

		auto GetRenderArea()		  const -> const FVulkanRenderArea& { return RenderArea; }
		auto GetSubpasses()			  const -> const Array<FSubpass>& { return Subpasses; }
		auto GetRenderPassBeginInfo() const -> const VkRenderPassBeginInfo;
		auto GetHandle()			  const	-> const VkRenderPass { return Handle; }

		Bool HasSubpass(SharedPtr<const FVulkanRenderPipeline> _SubpassPipeline) const;
	
	protected:
		ERenderPassType					Type;
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		FVulkanRenderArea				RenderArea{ 0,0 };
		FVulkanRenderPassLayout			Layout;
		Array<FClearValue>				ClearValues;
		Array<FVulkanFramebuffer>		Framebuffers;
		Array<FSubpass>					Subpasses;

	private:
		void Create(const FVulkanRenderArea& _RenderArea, const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets);
		void Destroy();

	public:
		FVulkanRenderPass() = delete;
		FVulkanRenderPass(ERenderPassType _RenderPassType);
		~FVulkanRenderPass() noexcept { Destroy(); }
	};

	FVulkanRenderPass::
	FVulkanRenderPass(ERenderPassType _RenderPassType)
		:Type{_RenderPassType}
	{
		switch (Type)
		{
		case ERenderPassType::Background:
		{
			Layout.AddColorAttachment(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Clear,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ColorAttachment,
				.FinalLayout	= EVulkanImageLayout::ColorAttachment,
			});
			break;
		}
		case ERenderPassType::DefaultForward:
		{
			Layout.AddColorAttachment(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Keep,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ColorAttachment,
				.FinalLayout	= EVulkanImageLayout::ColorAttachment,
			});
			break;
		}
		case ERenderPassType::Overlay:
		{
			Layout.AddColorAttachment(
			{
				.Layout			= EVulkanImageLayout::ColorAttachment,
				.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
				.SampleRate		= EVulkanSampleRate::X1,
				.ViewType		= EVulkanImageViewType::Image2D,
				.LoadOp			= EVulkanAttachmentIO::I_Keep,
				.StoreOp		= EVulkanAttachmentIO::O_Store,
				.InitialLayout	= EVulkanImageLayout::ColorAttachment,
				.FinalLayout	= EVulkanImageLayout::TransferSource,//[FIXME]
			});
			break;
		}
		case ERenderPassType::Customized:
		{
			break;
		}
		default: throw SRuntimeError("Unkonwn RenderPassLayout Preset!");
		}

		//[TODO]: Currently, It has to load a depth image.
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
	}

	void FVulkanRenderPass::
	Create(const FVulkanRenderArea& _RenderArea,
		   const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets)
	{
		VE_ASSERT(_RenderTargets.size() == GVulkan->Swapchain->GetFrameCount());
		VE_ASSERT(_RenderArea.extent.width > 0 && _RenderArea.extent.height > 0);
		VE_ASSERT(!Subpasses.empty());

		RenderArea = _RenderArea;
		
		// Create RenderPass
		Array<VkSubpassDescription> SubpassDescriptions(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencies(Subpasses.size());
		Array<Array<VkAttachmentReference>> ColorRefTable(Subpasses.size());
		Array<Array<VkAttachmentReference>> ResolveRefTable(Subpasses.size());
		
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
	
			Optional<VkAttachmentReference> DepthRef;
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
				.pDepthStencilAttachment= &DepthRef.value(),
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
				if (CurrentSubpass.VertexShader.expired())
				{ throw SRuntimeError("WIP: Cannot reload the vertex shader!"); }
				if (CurrentSubpass.FragmentShader.expired())
				{ throw SRuntimeError("WIP: Cannot reload the fragment shader!"); }

				CurrentSubpass.Pipeline->Create(Handle, Idx,
											CurrentSubpass.VertexShader.lock(),
											CurrentSubpass.FragmentShader.lock());
			}
			else if (CurrentSubpass.bExternalSubpass)
			{
				
			}
			else { throw SRuntimeError("Cannot Create a Subpass without any Pipeline!"); }
		}

		// Create Framebuffers
		Framebuffers.resize(_RenderTargets.size());
		for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
		{
			FVulkanExtent3D Extent{ RenderArea.extent.width, RenderArea.extent.height, 1 };
			Framebuffers[Idx].Create(Handle, Extent, _RenderTargets[Idx]);
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