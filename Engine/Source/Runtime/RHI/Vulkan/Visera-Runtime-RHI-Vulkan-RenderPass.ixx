module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPass;
import :Common;
import :Allocator;
import :Device;
import :Swapchain;
import :PipelineCache;
import :Framebuffer;
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
		struct FSubpass final
		{
			UniquePtr<FVulkanRenderPipeline>Pipeline;

			Array<UInt8>					ColorImageReferences;
			//Optional<VkAttachmentReference> StencilImageReference;
			EPipelineStage					SourceStage					 { EPipelineStage::None };
			EAccessibility					SourceStageAccessibility	 { EAccessibility::None };
			EPipelineStage					DestinationStage			 { EPipelineStage::None };
			EAccessibility					DestinationStageAccessibility{ EAccessibility::None };
			Bool							bEnableDepthTest = True;
			Array<UInt8>					InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt8>					PreserveImageReferences; // Const Image References Used in Subpasses.
		};

		auto GetHandle() const -> const VkRenderPass { return Handle; }
	
	protected:
		VkRenderPass						Handle{ VK_NULL_HANDLE };
		UInt32								Priority { 0 }; // Less is More
		SharedPtr<FVulkanRenderPassLayout>	Layout;
		Array<FVulkanFramebuffer>			Framebuffers;
		Array<FSubpass>						Subpasses;

	//private:
		// Created by RHI Module (User : Render Module )
		void Create(const Array<SharedPtr<FVulkanRenderPassResource>>& _RenderTargetsPackage);
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = default;
		~FVulkanRenderPass() noexcept { Destroy(); }
	};

	void FVulkanRenderPass::
	Create(const Array<SharedPtr<FVulkanRenderPassResource>>& _RenderTargetsPackage)
	{
		VE_ASSERT(_RenderTargetsPackage.size() == GVulkan->Swapchain->GetSize());

		// Create Subpasses (Setted in the derived class)
		if (Subpasses.empty()) { throw SRuntimeError("Cannot create a RenderPass with 0 Subpass!"); }
		for (auto& Subpass : Subpasses)
		{
			VE_ASSERT(Subpass.Pipeline != nullptr);
			Subpass.Pipeline->Create(Handle);
		}

		// Create RenderPass
		Array<VkSubpassDescription> SubpassDescriptions(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencies(Subpasses.size());

		for (UInt32 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& CurrentSubpass = Subpasses[Idx];

			Array<VkAttachmentReference> ColorAttachmentRefs(CurrentSubpass.ColorImageReferences.size());
			Array<VkAttachmentReference> ResolveAttachmentRefs(ColorAttachmentRefs.size());
			for (UInt8 Idx = 0; Idx < ColorAttachmentRefs.size(); ++Idx)
			{
				ColorAttachmentRefs[Idx].attachment = CurrentSubpass.ColorImageReferences[Idx];
				ColorAttachmentRefs[Idx].layout = AutoCast(
					_RenderTargetsPackage.front()
					->ColorImages.front()
					->GetLayout());
				ResolveAttachmentRefs[Idx].attachment = Layout->GetColorAttachmentCount() + CurrentSubpass.ColorImageReferences[Idx];
				ResolveAttachmentRefs[Idx].layout = AutoCast(
					_RenderTargetsPackage.front()
					->ResolveImages.front()
					->GetLayout());
			}
			VE_ASSERT(Layout->HasDepthImage()); // Debugging
			VkAttachmentReference DepthAttachmentRef
			{
				.attachment = Layout->GetDepthAttachmentLocation(),
				.layout = AutoCast(
					_RenderTargetsPackage.front()
					->DepthImage->GetLayout())
			};

			SubpassDescriptions[Idx] =
			{
				.flags					= 0x0,
				.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount	= 0, //[FIXME]
				.pInputAttachments		= nullptr,
				.colorAttachmentCount	= UInt32(ColorAttachmentRefs.size()),
				.pColorAttachments		= ColorAttachmentRefs.data(),
				.pResolveAttachments	= ResolveAttachmentRefs.data(),
				.pDepthStencilAttachment= &DepthAttachmentRef,
				.preserveAttachmentCount= 0,
				.pPreserveAttachments	= nullptr,
			};

			static_assert(VK_SUBPASS_EXTERNAL == (0U - 1));
			SubpassDependencies[Idx] =
			{
				.srcSubpass		= Idx - 1,
				.dstSubpass		= Idx,
				.srcStageMask	= AutoCast(CurrentSubpass.SourceStage),
				.dstStageMask	= AutoCast(CurrentSubpass.DestinationStage),
				.srcAccessMask	= AutoCast(CurrentSubpass.SourceStageAccessibility),
				.dstAccessMask	= AutoCast(CurrentSubpass.DestinationStageAccessibility),
				.dependencyFlags= 0x0,
			};
		}
		
		auto AttachmentDescriptions = Array<VkAttachmentDescription>(2 * Layout->GetColorAttachmentCount() + (Layout->HasDepthImage()? 1 : 0));
		
		for (UInt8 Idx = 0; Idx < Layout->GetColorAttachmentCount(); ++Idx)
		{
			auto& ColorDesc    = Layout->ColorDescs[Idx];
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
			auto& ResolveDesc   = Layout->ResolveDescs[Idx];
			AttachmentDescriptions[Layout->GetColorAttachmentCount() + Idx] = VkAttachmentDescription
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
		// Depth Attachment
		VE_ASSERT(Layout->HasDepthImage()); // Current Visera Pipeline will automatically create depth image!
		if (Layout->HasDepthImage())
		{
			auto& DepthDesc   = Layout->DepthDesc.value();
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

		Framebuffers.resize(_RenderTargetsPackage.size());
		for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
		{
			Framebuffers[Idx].Create(Handle, Layout->RenderAreaExtent, _RenderTargetsPackage[Idx]);
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
			{ Subpass.Pipeline->Destroy(); }

			vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

} } // namespace VE::Runtime