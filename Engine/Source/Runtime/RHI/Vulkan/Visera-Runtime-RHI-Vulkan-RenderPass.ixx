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
import :RenderTarget;

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
			WeakPtr<FVulkanShader>			VertexShader;
			WeakPtr<FVulkanShader>			FragmentShader;

			Array<UInt8>					ColorImageReferences;
			//Optional<VkAttachmentReference> StencilImageReference;
			EVulkanPipelineStage			BeginStage				{ EVulkanPipelineStage::None };
			EVulkanAccessibility			BeginStageAccessibility	{ EVulkanAccessibility::None };
			EVulkanPipelineStage			EndStage				{ EVulkanPipelineStage::None };
			EVulkanAccessibility			EndStageAccessibility	{ EVulkanAccessibility::None };
			Bool							bEnableDepthTest		= True;
			Array<UInt8>					InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt8>					PreserveImageReferences; // Const Image References Used in Subpasses.
		};
	
		auto GetRenderPassBeginInfo()  const -> VkRenderPassBeginInfo;
		auto GetHandle()	const	-> const VkRenderPass { return Handle; }
	
	protected:
		VkRenderPass						Handle{ VK_NULL_HANDLE };
		UInt32								Priority { 0 }; // Less is More
		SharedPtr<FVulkanRenderPassLayout>	Layout; // Slang Reflect after Create(...)
		Array<FVulkanFramebuffer>			Framebuffers;
		Array<FSubpass>						Subpasses;

	//private:
		// Created by RHI Module (User : Render Module )
		void Create(const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets);
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = default;
		~FVulkanRenderPass() noexcept { Destroy(); }
	};

	void FVulkanRenderPass::
	Create(const Array<SharedPtr<FVulkanRenderTarget>>& _RenderTargets)
	{
		VE_ASSERT(_RenderTargets.size() == GVulkan->Swapchain->GetFrameCount());

		// Create RenderPass
		Array<VkSubpassDescription> SubpassDescriptions(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencies(Subpasses.size());
		Array<Array<VkAttachmentReference>> ColorRefTable(Subpasses.size());
		Array<Array<VkAttachmentReference>> ResolveRefTable(Subpasses.size());

		for (UInt32 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& CurrentSubpass = Subpasses[Idx];

			auto& ColorRefs = ColorRefTable[Idx];
			ColorRefs.resize(CurrentSubpass.ColorImageReferences.size());
			auto& ResolveRefs = ResolveRefTable[Idx];
			ResolveRefs.resize(ColorRefs.size());
			for (UInt8 Idx = 0; Idx < ColorRefs.size(); ++Idx)
			{
				ColorRefs[Idx].attachment	= CurrentSubpass.ColorImageReferences[Idx];
				ColorRefs[Idx].layout		= AutoCast(Layout->ColorDescs[Idx].Layout);
				ResolveRefs[Idx].attachment = Layout->GetColorAttachmentCount() + CurrentSubpass.ColorImageReferences[Idx];
				ResolveRefs[Idx].layout		= AutoCast(Layout->ResolveDescs[Idx].Layout);
			}
			VE_ASSERT(Layout->HasDepthImage()); // Debugging
			VkAttachmentReference DepthRef
			{
				.attachment = Layout->GetDepthAttachmentLocation(),
				.layout = AutoCast(Layout->DepthDesc.value().Layout),
			};

			SubpassDescriptions[Idx] =
			{
				.flags					= 0x0,
				.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount	= 0, //[FIXME]
				.pInputAttachments		= nullptr,
				.colorAttachmentCount	= UInt32(ColorRefs.size()),
				.pColorAttachments		= ColorRefs.data(),
				.pResolveAttachments	= ResolveRefs.data(),
				.pDepthStencilAttachment= &DepthRef,
				.preserveAttachmentCount= 0,
				.pPreserveAttachments	= nullptr,
			};

			static_assert(VK_SUBPASS_EXTERNAL == (0U - 1));
			SubpassDependencies[Idx] =
			{
				.srcSubpass		= Idx - 1,
				.dstSubpass		= Idx,
				.srcStageMask	= AutoCast(CurrentSubpass.BeginStage),
				.dstStageMask	= AutoCast(CurrentSubpass.EndStage),
				.srcAccessMask	= AutoCast(CurrentSubpass.BeginStageAccessibility),
				.dstAccessMask	= AutoCast(CurrentSubpass.EndStageAccessibility),
				.dependencyFlags= 0x0,
			};
		}
		
		auto AttachmentDescriptions = Array<VkAttachmentDescription>(Layout->GetTotalAttachmentCount());
		
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

		// Create Subpasses (Setted in the derived class)
		if (Subpasses.empty()) { throw SRuntimeError("Cannot create a RenderPass with 0 Subpass!"); }
		
		for (auto& Subpass : Subpasses)
		{
			VE_ASSERT(Subpass.Pipeline != nullptr);
			if (Subpass.VertexShader.expired())
			{ throw SRuntimeError("WIP: Cannot reload the vertex shader!"); }
			if (Subpass.FragmentShader.expired())
			{ throw SRuntimeError("WIP: Cannot reload the fragment shader!"); }

			Subpass.Pipeline->Create(Handle, Subpass.VertexShader.lock(),
											 Subpass.FragmentShader.lock());
		}

		// Create Framebuffers
		Framebuffers.resize(_RenderTargets.size());
		for(UInt8 Idx = 0; Idx < Framebuffers.size(); ++Idx)
		{
			FVulkanExtent3D Extent{ Layout->RenderArea.extent.width, Layout->RenderArea.extent.height, 1 };
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
			{ Subpass.Pipeline->Destroy(); }

			vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

	VkRenderPassBeginInfo FVulkanRenderPass::
	GetRenderPassBeginInfo() const
	{
		return VkRenderPassBeginInfo
		{
			.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext			 = nullptr,
			.renderPass		 = Handle,
			.framebuffer	 = Framebuffers[GVulkan->Swapchain->GetCursor()].Handle,
			.renderArea		 = Layout->GetRenderArea(),
			.clearValueCount = UInt32(Layout->ClearColors.size()),
			.pClearValues    = Layout->ClearColors.data(),
		};
	}

} } // namespace VE::Runtime