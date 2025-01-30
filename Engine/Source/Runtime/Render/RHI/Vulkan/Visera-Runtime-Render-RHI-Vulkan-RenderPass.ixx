module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPass;

import Visera.Core.Signal;

import :Enums;
import :Device;
import :Shader;
import :CommandPool;
import :Swapchain;
import :PipelineCache;
import :Framebuffer;
import :RenderPipeline;

export namespace VE { namespace Runtime
{


	class FVulkanRenderPass
	{
		friend class FVulkan;
		class Subpass;
	public:
		virtual void Start(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> CommandBuffer) const;
		virtual void Render(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> CommandBuffer) = 0;
		virtual void Stop(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer>  CommandBuffer) const;

		auto GetSubpasses() const -> const Array<UniquePtr<Subpass>>&  { return Subpasses; }
		auto GetHandle()	const -> VkRenderPass			{ return Handle; }
		operator VkRenderPass()	const { return Handle; }

		class FSubpass
		{
			friend class FVulkanRenderPass;
			void Create(const FVulkanRenderPass& HostPass);
			void Destory(const FVulkanRenderPass& HostPass);

			VkPipeline						 Handle { VK_NULL_HANDLE };
			SharedPtr<FVulkanRenderPipeline> Pipeline;
			
			FSubpass() = delete;
			FSubpass(SharedPtr<FVulkanRenderPipeline> _Pipeline) : Pipeline{ std::move(_Pipeline) } {}
		};

	protected:
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		UInt32							Priority { 0 }; // Less is More
		VkRect2D						RenderArea; //Default: {{0,0}, {Swapchain.Extent}}
		Array<FVulkanFramebuffer>		FrameBuffers;
		Array<VkAttachmentDescription>  Attachments;

		struct SubpassDescription
		{
			struct AttachmentReference{ // === VkAttachmentReference
				FVulkanFramebuffer::TextureIndex	Location = ~0U; // Index In the FrameBuffer
				EImageLayout Layout;
			};

			Array<AttachmentReference>		InputAttachments;
			Array<AttachmentReference>		ColorAttachments;
			Segment<AttachmentReference,1>	DepthStencilAttachment;

			Array<AttachmentReference>		 ResolveAttachments;
			Array<FVulkanFramebuffer::TextureIndex> PreservveAttachments;
		};

		struct SubpassDependency
		{
			EPipelineStage	SourceStage;
			EAccessibility	SourceStageAccessPermissions;
			EPipelineStage	DestinationStage;
			EAccessibility	DestinationStageAccessPermissions;
		};
		Array<UniquePtr<Subpass>>		Subpasses;
		Array<SubpassDescription>		SubpassDescriptions;
		Array<SubpassDependency>		SubpassDependencies;

	public:
		void Create();
		void Destroy();

	public:
		FVulkanRenderPass() noexcept = delete;
		FVulkanRenderPass(UInt32 SubpassCount) noexcept;
		virtual ~FVulkanRenderPass() noexcept;
	};

	void FVulkanRenderPass::
	Start(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> CommandBuffer) const
	{
		VE_ASSERT(CommandBuffer->IsRecording());

		auto& CurrentFrameBuffer = FrameBuffers[GVulkan->Swapchain->GetCursor()];
		VkRenderPassBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass		= Handle,
			.framebuffer	= CurrentFrameBuffer.Handle,
			.renderArea		= RenderArea,
			.clearValueCount= UInt32(CurrentFrameBuffer.ClearColors.size()),
			.pClearValues	= CurrentFrameBuffer.ClearColors.data()
		};
		VkSubpassContents SubpassContents =
			CommandBuffer->IsPrimary() ?
			VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

		vkCmdBeginRenderPass(CommandBuffer->GetHandle(), &BeginInfo, SubpassContents);
	}

	void FVulkanRenderPass::
	Stop(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> CommandBuffer) const
	{
		VE_ASSERT(CommandBuffer->IsRecording());
		vkCmdEndRenderPass(CommandBuffer->GetHandle());
	}

	FVulkanRenderPass::
	FVulkanRenderPass(UInt32 SubpassCount) noexcept
		:RenderArea{{0,0}, { GVulkan->Swapchain->GetExtent() }},
			Subpasses(SubpassCount),
			SubpassDescriptions(SubpassCount),
			SubpassDependencies(SubpassCount),
			FrameBuffers(GVulkan->Swapchain->GetImages().size())
	{
		//!!!Remeber to call VulkanRenderPass::Create() in the Derived Renderpass!!!
	}

	FVulkanRenderPass::
	~FVulkanRenderPass() noexcept
	{
		Destroy();
	}

	void FVulkanRenderPass::
	Create()
	{
		VE_ASSERT(!Subpasses.empty() &&
				Subpasses.size() == SubpassDescriptions.size() == SubpassDependencies.size());
		
		Array<VkSubpassDescription> SubpassDescriptionInfos(Subpasses.size());
		Array<VkSubpassDependency>  SubpassDependencyInfos(Subpasses.size());

		for (UInt32 Idx = 0; Idx < Subpasses.size(); ++Idx)
		{
			auto& Description = SubpassDescriptions[Idx];
			SubpassDescriptionInfos[Idx] = VkSubpassDescription
			{
				.flags					= 0x0,
				.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount	= UInt32(Description.InputAttachments.size()),
				.pInputAttachments		= reinterpret_cast<VkAttachmentReference*>(Description.InputAttachments.data()),
				.colorAttachmentCount	= UInt32(Description.ColorAttachments.size()),
				.pColorAttachments		= reinterpret_cast<VkAttachmentReference*>(Description.ColorAttachments.data()),
				.pResolveAttachments	= reinterpret_cast<VkAttachmentReference*>(Description.ResolveAttachments.data()),
				.pDepthStencilAttachment= Description.DepthStencilAttachment[0].Location == ~0U? 
											VK_NULL_HANDLE : reinterpret_cast<VkAttachmentReference*>(Description.DepthStencilAttachment.data()),
				.preserveAttachmentCount= UInt32(Description.PreservveAttachments.size()),
				.pPreserveAttachments	= Description.PreservveAttachments.data(),
			};

			auto& Dependency = SubpassDependencies[Idx];
			VE_ASSERT(VK_SUBPASS_EXTERNAL == UInt32(0 - 1));
			SubpassDependencyInfos[Idx] = VkSubpassDependency
			{
				.srcSubpass = Idx - 1,
				.dstSubpass = Idx,
				.srcStageMask = AutoCast(Dependency.SourceStage),
				.dstStageMask = AutoCast(Dependency.DestinationStage),
				.srcAccessMask = AutoCast(Dependency.SourceStageAccessPermissions),
				.dstAccessMask = AutoCast(Dependency.DestinationStageAccessPermissions),
				.dependencyFlags = 0x0,
			};
		}

		VE_ASSERT(!Attachments.empty());
		VkRenderPassCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = UInt32(Attachments.size()),
			.pAttachments	 = Attachments.data(),
			.subpassCount	 = UInt32(SubpassDescriptionInfos.size()),
			.pSubpasses		 = SubpassDescriptionInfos.data(),
			.dependencyCount = UInt32(SubpassDependencyInfos.size()),
			.pDependencies	 = SubpassDependencyInfos.data(),
		};
		if(VK_SUCCESS != vkCreateRenderPass(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan FRenderPass!"); }
	
		//Create FrameBuffers
		for (auto& FrameBuffer : FrameBuffers)
		{
			VE_ASSERT(!FrameBuffer.RenderTargets.empty());
			VkFramebufferCreateInfo FrameBufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass			= Handle,
				.attachmentCount	= UInt32(FrameBuffer.RenderTargets.size()),
				.pAttachments		= FrameBuffer.RenderTargets.data(),
				.width  = RenderArea.extent.width,
				.height = RenderArea.extent.height,
				.layers = 1
			};
			if(VK_SUCCESS != vkCreateFramebuffer(
				GVulkan->Device->GetHandle(),
				&FrameBufferCreateInfo,
				GVulkan->AllocationCallbacks,
				&FrameBuffer.Handle))
			{ throw SRuntimeError("Failed to create Vulkan Framebuffer!"); }
		}
	}

	void FVulkanRenderPass::
	Destroy()
	{
		for (auto& Subpass : Subpasses) { Subpass->Destroy(); }
		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	void FVulkanRenderPass::FSubpass::
	Create(const FVulkanRenderPass& HostPass)
	{
		VE_ASSERT(HostPass.GetHandle() != VK_NULL_HANDLE &&
				  Pipeline != nullptr);
		
		VkPipelineDynamicStateCreateInfo DyncmicStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = UInt32(Pipeline->GetDynamicStates().size()),
			.pDynamicStates    = Pipeline->GetDynamicStates().data()
		};

		auto ViewportState = VkPipelineViewportStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount	= UInt32(Pipeline->GetViewports().size()),
			.pViewports		= Pipeline->GetViewports().data(),
			.scissorCount	= UInt32(Pipeline->GetScissors().size()),
			.pScissors		= Pipeline->GetScissors().data(),
		};

		auto ColorBlendState = VkPipelineColorBlendStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable		= VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
			.logicOp			= VK_LOGIC_OP_COPY,
			.attachmentCount	= UInt32(Pipeline->GetColorBlendAttachments().size()),
			.pAttachments		= Pipeline->GetColorBlendAttachments().data(),
			.blendConstants		= {0.0f, 0.0f, 0.0f, 0.0f}
		};

		VkGraphicsPipelineCreateInfo CreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount				= UInt32(Pipeline->GetShaderStages().size()),
			.pStages				= Pipeline->GetShaderStages().data(),
			.pVertexInputState		= &Pipeline->GetVertexInputState(),
			.pInputAssemblyState	= &Pipeline->GetInputAssemblyState(),
			.pViewportState			= &ViewportState,
			.pRasterizationState	= &Pipeline->GetRasterizationState(),
			.pMultisampleState		= &Pipeline->GetMultisampleState(),
			.pDepthStencilState		= &Pipeline->GetDepthStencilState(),	// Optional
			.pColorBlendState		= &ColorBlendState,
			.pDynamicState			= &DyncmicStateCreateInfo,
			.layout					= Pipeline->GetLayout(),
			.renderPass				= HostPass.GetHandle(),
			.basePipelineHandle		= VK_NULL_HANDLE,		// Optional
			.basePipelineIndex		= -1,					// Optional
		};
		
		if(VK_SUCCESS != vkCreateGraphicsPipelines(
			GVulkan->Device->GetHandle(),
			GVulkan->GraphicsPipelineCache->GetHandle(),
			1,
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan Graphics Pipeline!"); }
	}

	void FVulkanRenderPass::FSubpass::
	Destory(const FVulkanRenderPass& HostPass)
	{
		vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime