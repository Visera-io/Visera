module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPass;
import :Common;
import :Allocator;
import :Device;
import :PipelineCache;
import :Framebuffer;
import :RenderPipeline;
import :RenderPassLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanRenderPass
	{
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
		auto GetHandle() const -> const VkRenderPass { return Handle; }
	
	protected:
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		UInt32							Priority { 0 }; // Less is More
		FVulkanRenderPassLayout			Layout;
		FVulkanFramebuffer				Framebuffer;
		Array<FSubpass>					Subpasses;

		void Create();
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = default;
		virtual ~FVulkanRenderPass() noexcept;
	};

	FVulkanRenderPass::
	~FVulkanRenderPass() noexcept
	{
		Destroy();
	}

	void FVulkanRenderPass::
	Create()
	{
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
		
		Array<VkAttachmentDescription> AttachmentDescriptions(Layout.AttachmentCount);
		for (UInt8 Idx = 0; Idx < AttachmentDescriptions.size(); ++Idx)
		{
			auto& Description = Layout.AttachmentDescriptions[Idx];
			AttachmentDescriptions[Idx] = VkAttachmentDescription
			{ 
				.flags			= 0x0,
				.format			= AutoCast(Description.Image->GetFormat()),
				.samples		= AutoCast(AutoCast(Description.Image->GetSampleRate())),
				.loadOp			= AutoCast(Description.LoadOp),
				.storeOp		= AutoCast(AutoCast(Description.StoreOp)),
				.stencilLoadOp	= AutoCast(Description.StencilLoadOp),
				.stencilStoreOp = AutoCast(AutoCast(Description.StencilStoreOp)),
				.initialLayout	= AutoCast(Description.InitialLayout),
				.finalLayout	= AutoCast(Description.FinalLayout),
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

		// Create Framebuffers
		Framebuffer.Create(Layout);
	}

	void FVulkanRenderPass::
	Destroy()
	{
		for (auto& Subpass : Subpasses)
		{ Subpass.Pipeline.Destroy(); }
		
		Framebuffer.Destroy();

		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime