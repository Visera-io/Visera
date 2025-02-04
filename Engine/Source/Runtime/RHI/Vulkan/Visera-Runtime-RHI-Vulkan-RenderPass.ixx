module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPass;

import Visera.Core.Signal;

import :Enums;
import :Device;
import :PipelineCache;
import :Framebuffer;
import :RenderPipeline;
import :RenderPassLayout;

export namespace VE { namespace Runtime
{

	class FVulkanRenderPass
	{
	public:
		class FSubpass
		{
			friend class FVulkanRenderPass;
			FVulkanRenderPipeline	Pipeline;
			Array<VkAttachmentReference>	InputImageReferences;    // Input Image References from Previous Subpasses.
			Array<UInt32>					PreserveImageReferences; // Const Image References Used in Subpasses.
			Array<VkAttachmentReference>	ColorImageReferences;
			Array<VkAttachmentReference>	ResolveImageReferences;
			Optional<VkAttachmentReference> DepthImageReference;
			Optional<VkAttachmentReference> StencilImageReference;
			EPipelineStage			SourceStage							{ EPipelineStage::None };
			EAccessibility			SourceStageAccessPermissions		{ EAccessibility::None };
			EPipelineStage			DestinationStage					{ EPipelineStage::None };
			EAccessibility			DestinationStageAccessPermissions	{ EAccessibility::None };
		};
		auto GetHandle() -> VkRenderPass { return Handle; }
	
	protected:
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		UInt32							Priority { 0 }; // Less is More
		FVulkanRenderPassLayout			Layout;
		Array<FSubpass>					Subpasses;
		FVulkanFramebuffer				Framebuffer;

	public:
		void Create();
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = delete;
		FVulkanRenderPass(UInt32 SubpassCount) noexcept;
		virtual ~FVulkanRenderPass() noexcept;
	};

	FVulkanRenderPass::
	FVulkanRenderPass(UInt32 SubpassCount) noexcept
	{
		VE_WIP;
	}

	FVulkanRenderPass::
	~FVulkanRenderPass() noexcept
	{
		Destroy();
	}

	void FVulkanRenderPass::
	Create()
	{
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

			static_assert(VK_SUBPASS_EXTERNAL == UInt32(0 - 1));
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

		VE_WIP;
		VkRenderPassCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = UInt32(Framebuffer.ColorImages.size()),
			//.pAttachments	 = Attachments.data(),
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
		{ throw SRuntimeError("Failed to create Vulkan FRenderPass!"); }

		// Create Framebuffers

		/*VkFramebufferCreateInfo CreateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass		= Handle,
			.attachmentCount= UInt32(Framebuffer.Color.size()),
			.pAttachments = m_framebuffers[i].render_targets.data(),
			.width		  = m_render_area.extent.width,
			.height		  = m_render_area.extent.height,
			.layers		  = 1
		};*/
	}

	void FVulkanRenderPass::
	Destroy()
	{
		VE_WIP;
		//for (auto& Subpass : Subpasses) { Subpass->Destroy(); }
		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	//void FVulkanRenderPass::FSubpass::
	//Create(const FVulkanRenderPass& HostPass)
	//{
	//	VE_ASSERT(HostPass.GetHandle() != VK_NULL_HANDLE &&
	//			  Pipeline != nullptr);
	//	
	//	VkPipelineDynamicStateCreateInfo DyncmicStateCreateInfo
	//	{
	//		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	//		.dynamicStateCount = UInt32(Pipeline->GetDynamicStates().size()),
	//		.pDynamicStates    = Pipeline->GetDynamicStates().data()
	//	};

	//	auto ViewportState = VkPipelineViewportStateCreateInfo
	//	{
	//		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	//		.viewportCount	= UInt32(Pipeline->GetViewports().size()),
	//		.pViewports		= Pipeline->GetViewports().data(),
	//		.scissorCount	= UInt32(Pipeline->GetScissors().size()),
	//		.pScissors		= Pipeline->GetScissors().data(),
	//	};

	//	auto ColorBlendState = VkPipelineColorBlendStateCreateInfo
	//	{
	//		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	//		.logicOpEnable		= VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
	//		.logicOp			= VK_LOGIC_OP_COPY,
	//		.attachmentCount	= UInt32(Pipeline->GetColorBlendAttachments().size()),
	//		.pAttachments		= Pipeline->GetColorBlendAttachments().data(),
	//		.blendConstants		= {0.0f, 0.0f, 0.0f, 0.0f}
	//	};

	//	VE_WIP;
	//	VkGraphicsPipelineCreateInfo CreateInfo =
	//	{
	//		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	//		.stageCount				= UInt32(Pipeline->GetShaderStages().size()),
	//		.pStages				= Pipeline->GetShaderStages().data(),
	//		.pVertexInputState		= &Pipeline->GetVertexInputState(),
	//		.pInputAssemblyState	= &Pipeline->GetInputAssemblyState(),
	//		.pViewportState			= &ViewportState,
	//		.pRasterizationState	= &Pipeline->GetRasterizationState(),
	//		.pMultisampleState		= &Pipeline->GetMultisampleState(),
	//		.pDepthStencilState		= &Pipeline->GetDepthStencilState(),	// Optional
	//		.pColorBlendState		= &ColorBlendState,
	//		.pDynamicState			= &DyncmicStateCreateInfo,
	//		//.layout					= Pipeline->GetLayout(),
	//		.renderPass				= HostPass.GetHandle(),
	//		.basePipelineHandle		= VK_NULL_HANDLE,		// Optional
	//		.basePipelineIndex		= -1,					// Optional
	//	};
	//	
	//	if(VK_SUCCESS != vkCreateGraphicsPipelines(
	//		GVulkan->Device->GetHandle(),
	//		GVulkan->GraphicsPipelineCache->GetHandle(),
	//		1,
	//		&CreateInfo,
	//		GVulkan->AllocationCallbacks,
	//		&Handle))
	//	{ throw SRuntimeError("Failed to create Vulkan Graphics Pipeline!"); }
	//}

	/*void FVulkanRenderPass::FSubpass::
	Destory(const FVulkanRenderPass& HostPass)
	{
		vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}*/

} } // namespace VE::Runtime