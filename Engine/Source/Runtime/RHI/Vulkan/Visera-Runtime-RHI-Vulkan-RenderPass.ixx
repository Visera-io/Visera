module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPass;
import :Common;
import :Allocator;
import :Device;
import :PipelineCache;
import :RenderPipeline;
import :RenderPassLayout;
import :RenderPassResource;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class RHI;

	class FVulkanRenderPass
	{
		friend class RHI;
	public:
		struct FSubpass
		{
			FVulkanRenderPipeline			Pipeline;

			Array<VkAttachmentReference>	InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt32>					PreserveImageReferences; // Const Image References Used in Subpasses.
			Array<VkAttachmentReference>	ColorImageReferences;
			Array<VkAttachmentReference>	ResolveImageReferences;
			Optional<VkAttachmentReference> DepthImageReference;
			Optional<VkAttachmentReference> StencilImageReference;
			EPipelineStage					SourceStage							{ EPipelineStage::None };
			EAccessibility					SourceStageAccessPermissions		{ EAccessibility::None };
			EPipelineStage					DestinationStage					{ EPipelineStage::None };
			EAccessibility					DestinationStageAccessPermissions	{ EAccessibility::None };
		};
		struct FFramebuffer
		{
			VkFramebuffer						Handle{ VK_NULL_HANDLE };
			SharedPtr<FVulkanRenderPassResource>RenderResource;
			Array<VkImageView>					RenderResourceViews;
			//[TODO]: Shading Rate Image & VkAttachmentDescriptionStencilLayout StencilDescription;
			Bool HasDepthImage() const { return RenderResource->HasDepthImage(); }
			~FFramebuffer();
		};

		auto GetHandle() const -> const VkRenderPass { return Handle; }
	
	protected:
		VkRenderPass						 Handle{ VK_NULL_HANDLE };
		UInt32								 Priority { 0 }; // Less is More
		SharedPtr<FVulkanRenderPassLayout>	 Layout;
		Array<FFramebuffer>					 Framebuffers;
		Array<FSubpass>						 Subpasses;

	private:
		// Created by RHI
		void Create();
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = default;
		virtual ~FVulkanRenderPass() noexcept;
	};

	FVulkanRenderPass::
	~FVulkanRenderPass() noexcept
	{
		if (Handle != VK_NULL_HANDLE) { Destroy(); }
	}

	void FVulkanRenderPass::
	Create()
	{
		VE_ASSERT(Layout->GetColorAttachmentCount() <= Framebuffer.RenderResource->ColorImages.size());
		VE_ASSERT(!Layout->HasDepthImage() ||
				 ( Layout->HasDepthImage() && Framebuffer.RenderResource->HasDepthImage()));

		// Create Subpasses
		for (auto& Subpass : Subpasses)
		{ Subpass.Pipeline.Create(); }

		// Create RenderPass
		Array<VkSubpassDescription> SubpassDescriptions(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencies(Subpasses.size());

		for (UInt32 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& CurrentSubpass = Subpasses[Idx];

			SubpassDescriptions[Idx] =
			{
				.flags					= 0x0,
				.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount	= UInt32(CurrentSubpass.InputImageReferences.size()),
				.pInputAttachments		= CurrentSubpass.InputImageReferences.data(),
				.colorAttachmentCount	= UInt32(CurrentSubpass.ColorImageReferences.size()),
				.pColorAttachments		= CurrentSubpass.ColorImageReferences.data(),
				.pResolveAttachments	= CurrentSubpass.ResolveImageReferences.data(),
				.pDepthStencilAttachment= CurrentSubpass.DepthImageReference.has_value()? &(CurrentSubpass.DepthImageReference.value()) : nullptr,
				.preserveAttachmentCount= UInt32(CurrentSubpass.PreserveImageReferences.size()),
				.pPreserveAttachments	= CurrentSubpass.PreserveImageReferences.data(),
			};

			static_assert(VK_SUBPASS_EXTERNAL == (0U - 1));
			SubpassDependencies[Idx] =
			{
				.srcSubpass		= Idx - 1,
				.dstSubpass		= Idx,
				.srcStageMask	= AutoCast(CurrentSubpass.SourceStage),
				.dstStageMask	= AutoCast(CurrentSubpass.DestinationStage),
				.srcAccessMask	= AutoCast(CurrentSubpass.SourceStageAccessPermissions),
				.dstAccessMask	= AutoCast(CurrentSubpass.DestinationStageAccessPermissions),
				.dependencyFlags= 0x0,
			};
		}
		
		auto AttachmentDescriptions = Array<VkAttachmentDescription>(2 * Layout->GetColorAttachmentCount() + (Layout->HasDepthImage()? 1 : 0));
		
		for (UInt8 Idx = 0; Idx < Layout->GetColorAttachmentCount(); ++Idx)
		{
			auto& ColorDesc    = Layout->ColorDescs[Idx];
			auto& RenderTarget = Framebuffer.RenderResource->ColorImages[Idx];
			
			// Color Attachments
			AttachmentDescriptions[Idx] = VkAttachmentDescription
			{ 
				.flags			= 0x0,
				.format			= AutoCast(RenderTarget->GetFormat()),
				.samples		= AutoCast(RenderTarget->GetSampleRate()),
				.loadOp			= AutoCast(ColorDesc.LoadOp),
				.storeOp		= AutoCast(AutoCast(ColorDesc.StoreOp)),
				.stencilLoadOp	= AutoCast(ColorDesc.StencilLoadOp),
				.stencilStoreOp = AutoCast(AutoCast(ColorDesc.StencilStoreOp)),
				.initialLayout	= AutoCast(ColorDesc.InitialLayout),
				.finalLayout	= AutoCast(ColorDesc.FinalLayout),
			};
			// Resolve Attachments
			auto& ResolveDesc   = Layout->ResolveDescs[Idx];
			auto& ResolveTarget = Framebuffer.RenderResource->ResolveImages[Idx];
			AttachmentDescriptions[Layout->GetColorAttachmentCount() + Idx] = VkAttachmentDescription
			{ 
				.flags			= 0x0,
				.format			= AutoCast(RenderTarget->GetFormat()),
				.samples		= AutoCast(RenderTarget->GetSampleRate()),
				.loadOp			= AutoCast(ResolveDesc.LoadOp),
				.storeOp		= AutoCast(AutoCast(ResolveDesc.StoreOp)),
				.stencilLoadOp	= AutoCast(ResolveDesc.StencilLoadOp),
				.stencilStoreOp = AutoCast(AutoCast(ResolveDesc.StencilStoreOp)),
				.initialLayout	= AutoCast(ResolveDesc.InitialLayout),
				.finalLayout	= AutoCast(ResolveDesc.FinalLayout),
			};
		}
		// Depth Attachment
		VE_ASSERT(Layout->HasDepthImage()); // Current Visera Pipeline will automatically create depth image!
		if (Layout->HasDepthImage())
		{
			auto& DepthDesc   = Layout->DepthDesc.value();
			auto& DepthTarget = Framebuffer.RenderResource->DepthImage;
			AttachmentDescriptions.back() = VkAttachmentDescription
			{
				.flags			= 0x0,
				.format			= AutoCast(DepthTarget->GetFormat()),
				.samples		= AutoCast(DepthTarget->GetSampleRate()),
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

		// Create Framebuffer
		Framebuffer.RenderResourceViews.resize(Layout->GetTotalAttachmentCount());
		VE_ASSERT(Framebuffer.RenderResourceViews.size() % 1 == 1); // Currently, Visera's each renderpass must have 2N + 1 attachments.
		
		for (UInt8 Idx = 0; Idx < Layout->GetColorAttachmentCount(); Idx++)
		{
			auto& ColorDesc = Layout->ColorDescs[Idx];
			auto& ColorView = Framebuffer.RenderResourceViews[Idx];
			ColorView		= Framebuffer.RenderResource->ColorImages[Idx]
							  ->CreateImageView(ColorDesc.ImageViewType)
							  ->Release();
			
			auto& ResolveDesc = Layout->ResolveDescs[Idx];
			auto& ResolveView = Framebuffer.RenderResourceViews[Layout->GetColorAttachmentCount() + Idx];
			ResolveView		  = Framebuffer.RenderResource->ResolveImages[Idx]
								->CreateImageView(ResolveDesc.ImageViewType)
								->Release();
		}

		if (Layout->HasDepthImage())
		{
			auto& DepthDesc = Layout->DepthDesc.value();
			auto& DepthView = Framebuffer.RenderResourceViews.back();
			DepthView = Framebuffer.RenderResource->DepthImage
						->CreateImageView(DepthDesc.ImageViewType)
						->Release();
		}

		if (Layout->StencilDesc.has_value())
		{
			VE_WIP;
			/*DepthStencilImage = GVulkan->Allocator->CreateImage(EImageType::Image2D,
																_OwnerLayout->GetExtent(),
																EImageAspect::Depth | EImageAspect::Stencil,
																EImageUsage::DepthStencilAttachment);*/
		}

		VkFramebufferCreateInfo FramebufferCreateInfo
		{
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext			 = nullptr,
			.flags			 = 0x0,
			.renderPass		 = Handle,
			.attachmentCount = UInt32(Framebuffer.RenderResourceViews.size()),
			.pAttachments	 = Framebuffer.RenderResourceViews.data(),
			.width  = Layout->GetRenderAreaExtent().width,
			.height = Layout->GetRenderAreaExtent().height,
			.layers = Layout->GetRenderAreaExtent().depth,
		};

		if (vkCreateFramebuffer(
			GVulkan->Device->GetHandle(),
			&FramebufferCreateInfo,
			GVulkan->AllocationCallbacks,
			&Framebuffer.Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Framebuffer!"); }
	}

	void FVulkanRenderPass::
	Destroy()
	{
		for (auto& Subpass : Subpasses)
		{ Subpass.Pipeline.Destroy(); }

		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	FVulkanRenderPass::FFramebuffer::
	~FFramebuffer()
	{ 
		for (auto& RenderTargetView : RenderResourceViews)
		{
			VE_ASSERT(RenderTargetView != VK_NULL_HANDLE);
			vkDestroyImageView(GVulkan->Device->GetHandle(), RenderTargetView, GVulkan->AllocationCallbacks);
		}
		vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}


} } // namespace VE::Runtime