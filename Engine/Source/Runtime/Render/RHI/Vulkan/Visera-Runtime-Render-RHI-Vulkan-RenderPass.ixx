module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPass;

import :Enums;
import :Device;
import :Shader;
import :CommandPool;
import :Swapchain;
import :PipelineCache;

VISERA_PUBLIC_MODULE

class VulkanRenderPass
{
	friend class Vulkan;
	class Subpass;
public:
	virtual void Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const;
	virtual void Render(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) = 0;
	virtual void Stop(SharedPtr<VulkanCommandPool::CommandBuffer>  CommandBuffer) const;
	//VkSubpassDependency
	struct FrameBuffer
	{
		using TextureIndex = UInt32;
		VkFramebuffer		Handle{ VK_NULL_HANDLE };
		Array<VkImageView>	RenderTargets;
		Array<VkClearValue>	ClearColors;

		operator VkFramebuffer() const { return Handle; }
		~FrameBuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }
	};

	auto GetSubpasses() const -> const Array<UniquePtr<Subpass>>&  { return Subpasses; }
	auto GetHandle()	const -> VkRenderPass			{ return Handle; }
	operator VkRenderPass()	const { return Handle; }
		
public:
	class Subpass
	{
		friend class VulkanRenderPass;
	public:
		//[TODO] UseSubpassCreateInfo
		void Create(const VulkanRenderPass& HostRenderPass, const Array<SharedPtr<VulkanShader>>& Shaders);
		void Destroy() noexcept;

		auto GetLayout()	const  ->	VkPipelineLayout			{ return Layout; }
		auto GetViewports() const  ->	const Array<VkViewport>&	{ return Viewports; }
		auto GetScissors()  const  ->	const Array<VkRect2D>&		{ return Scissors;  }
		auto GetColorBlendAttachments() const -> const Array<VkPipelineColorBlendAttachmentState>& { return ColorBlendAttachments; }
		auto GetHandle()		const  ->	VkPipeline { return Handle; }
		operator VkPipeline()	const  { return Handle; }

	protected:
		VkPipeline				Handle { VK_NULL_HANDLE };
		VkPipelineLayout		Layout { VK_NULL_HANDLE };

		Array<VkPipelineShaderStageCreateInfo>	ShaderStages;
		//[TODO]: Add Builders to subsitute CreateInfo
		/*1*/VkPipelineVertexInputStateCreateInfo	VertexInputState;
		/*2*/VkPipelineTessellationStateCreateInfo	TessellationState;
		/*3*/VkPipelineInputAssemblyStateCreateInfo	InputAssemblyState;
		/*4*/Array<VkViewport>						Viewports;    //Default(1)
				Array<VkRect2D>						Scissors;     //Default(1)
		/*5*/VkPipelineRasterizationStateCreateInfo	RasterizationState;
		/*6*/VkPipelineMultisampleStateCreateInfo	MultisampleState;
		/*7*/VkPipelineDepthStencilStateCreateInfo	DepthStencilState;
		/*8*/Array<VkPipelineColorBlendAttachmentState> ColorBlendAttachments; //Default(1)
		/*9*/Array<VkDynamicState>					DynamicStates{/*VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR*/};
	public:
		Subpass();
		~Subpass() noexcept { Destroy(); }
	};

protected:
	VkRenderPass					Handle{ VK_NULL_HANDLE };
	UInt32							Priority { 0 }; // Less is More
	VkRect2D						RenderArea; //Default: {{0,0}, {Swapchain.Extent}}
	Array<FrameBuffer>				FrameBuffers;
	Array<VkAttachmentDescription>  Attachments;

	struct SubpassDescription
	{
		struct AttachmentReference{ // === VkAttachmentReference
			FrameBuffer::TextureIndex	Location = ~0U; // Index In the FrameBuffer
			EImageLayout Layout;
		};

		Array<AttachmentReference>		InputAttachments;
		Array<AttachmentReference>		ColorAttachments;
		Segment<AttachmentReference,1>	DepthStencilAttachment;

		Array<AttachmentReference>		 ResolveAttachments;
		Array<FrameBuffer::TextureIndex> PreservveAttachments;
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
	VulkanRenderPass() noexcept = delete;
	VulkanRenderPass(UInt32 SubpassCount) noexcept;
	virtual ~VulkanRenderPass() noexcept;
};

void VulkanRenderPass::
Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const
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

void VulkanRenderPass::
Stop(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const
{
	VE_ASSERT(CommandBuffer->IsRecording());
	vkCmdEndRenderPass(CommandBuffer->GetHandle());
}

VulkanRenderPass::
VulkanRenderPass(UInt32 SubpassCount) noexcept
	:RenderArea{{0,0}, { GVulkan->Swapchain->GetExtent() }},
		Subpasses(SubpassCount),
		SubpassDescriptions(SubpassCount),
		SubpassDependencies(SubpassCount),
		FrameBuffers(GVulkan->Swapchain->GetImages().size())
{
	//!!!Remeber to call VulkanRenderPass::Create() in the Derived Renderpass!!!
}

VulkanRenderPass::
~VulkanRenderPass() noexcept
{
	Destroy();
}

void VulkanRenderPass::
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
	VK_CHECK(vkCreateRenderPass(GVulkan->Device->GetHandle(), &CreateInfo, GVulkan->AllocationCallbacks, &Handle));
	
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
		VK_CHECK(vkCreateFramebuffer(GVulkan->Device->GetHandle(), &FrameBufferCreateInfo, GVulkan->AllocationCallbacks, &FrameBuffer.Handle));
	}
}

void VulkanRenderPass::
Destroy()
{
	for (auto& Subpass : Subpasses) { Subpass->Destroy(); }
	vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

VulkanRenderPass::Subpass::
Subpass():
	VertexInputState{ VkPipelineVertexInputStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount	= 0,
		.pVertexBindingDescriptions		= nullptr,
		.vertexAttributeDescriptionCount= 0,
		.pVertexAttributeDescriptions	= nullptr,
	} },
	TessellationState{ VkPipelineTessellationStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		.patchControlPoints = 0, // Disabled
	} },
	InputAssemblyState{ VkPipelineInputAssemblyStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	} },
	Viewports{ {
		.x		= 0.0f, 
		.y		=	Float(GVulkan->Swapchain->GetExtent().height),
		.width	=	Float(GVulkan->Swapchain->GetExtent().width),
		.height	= - Float(GVulkan->Swapchain->GetExtent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,} },
	Scissors{{
		.offset = { 0, 0 },
		.extent = GVulkan->Swapchain->GetExtent(),
	}},
	RasterizationState{ VkPipelineRasterizationStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable		= VK_FALSE, // Fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
		.rasterizerDiscardEnable= VK_FALSE, // if VK_TRUE, then geometry never passes through the rasterizer stage
		.polygonMode			= VK_POLYGON_MODE_FILL,
		.cullMode				= VK_CULL_MODE_BACK_BIT,
		.frontFace				= VK_FRONT_FACE_CLOCKWISE, // Consistent with Unity Engine. (Note Y-Flip) [TODO]:Align with Unreal Engine
		.depthBiasEnable		= VK_TRUE,
		.depthBiasConstantFactor= 0.0f, 
		.depthBiasClamp			= 0.0f,
		.depthBiasSlopeFactor	= 0.0f, // This is sometimes used for shadow mapping
		.lineWidth				= 1.0f // Any line thicker than 1.0f requires you to enable the wideLines GPU feature.			
	} },
	MultisampleState{ VkPipelineMultisampleStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable  = VK_FALSE,
		.minSampleShading	  = 1.0f,
		.pSampleMask		  = nullptr,
		.alphaToCoverageEnable= VK_FALSE,
		.alphaToOneEnable	  = VK_FALSE,
	} },
	DepthStencilState{ VkPipelineDepthStencilStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		// Depth Test
		.depthTestEnable	= VK_FALSE,
		.depthWriteEnable	= VK_FALSE,
		.depthCompareOp		= VK_COMPARE_OP_LESS, // Keep fragments, which has lower depth
		.depthBoundsTestEnable= VK_FALSE,  // Only keep fragments that fall within the specified depth range.
		// Stencil Test
		.stencilTestEnable	= VK_FALSE,
		.front				= {},
		.back				= {},
		.minDepthBounds		= 0.0,
		.maxDepthBounds		= 1.0,
	} },
	ColorBlendAttachments{ {
		.blendEnable			= VK_TRUE,
		.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp			= VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp			= VK_BLEND_OP_ADD,
		.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT |
									VK_COLOR_COMPONENT_G_BIT | 
									VK_COLOR_COMPONENT_B_BIT | 
									VK_COLOR_COMPONENT_A_BIT,
	} }
{
	//!!!Remeber to call Subpass::Create() in Renderpass!!!
}

void VulkanRenderPass::Subpass::
Create(const VulkanRenderPass& HostRenderPass, const Array<SharedPtr<VulkanShader>>& Shaders)
{
	//[TODO][FIXME]: Add SPIR-V Reflection?
	VkPushConstantRange PCRange //[FIXME]: Test Data
	{
		.stageFlags = AutoCast(EShaderStage::Fragment),
		.offset = 0,
		.size = sizeof(float)
	};
	VkPipelineLayoutCreateInfo LayoutCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount			= 0,
		.pSetLayouts			= nullptr,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges	= &PCRange,
	};
	VK_CHECK(vkCreatePipelineLayout(GVulkan->Device->GetHandle(), &LayoutCreateInfo, GVulkan->AllocationCallbacks, &Layout));

	ShaderStages.resize(Shaders.size());
	for(UInt32 Idx = 0; Idx < ShaderStages.size(); ++Idx)
	{
		ShaderStages[Idx] = VkPipelineShaderStageCreateInfo
		{
			.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage  = Shaders[Idx]->GetStage(),
			.module = Shaders[Idx]->GetHandle(),
			.pName	= Shaders[Idx]->GetName(),
		};
	}

	VkPipelineDynamicStateCreateInfo DyncmicStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = UInt32(DynamicStates.size()),
		.pDynamicStates = DynamicStates.data()
	};

	auto ViewportState = VkPipelineViewportStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount	= UInt32(Viewports.size()),
		.pViewports		= Viewports.data(),
		.scissorCount	= UInt32(Scissors.size()),
		.pScissors		= Scissors.data(),
	};

	auto ColorBlendState = VkPipelineColorBlendStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable		= VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
		.logicOp			= VK_LOGIC_OP_COPY,
		.attachmentCount	= UInt32(ColorBlendAttachments.size()),
		.pAttachments		= ColorBlendAttachments.data(),
		.blendConstants		= {0.0f, 0.0f, 0.0f, 0.0f}
	};

	//!!!Call Subpass::Create() in the RenderPass::Create()!!!
	VE_ASSERT(HostRenderPass.GetHandle() != VK_NULL_HANDLE);
	VkGraphicsPipelineCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount				= UInt32(ShaderStages.size()),
		.pStages				= ShaderStages.data(),
		.pVertexInputState		= &VertexInputState,
		.pInputAssemblyState	= &InputAssemblyState,
		.pViewportState			= &ViewportState,
		.pRasterizationState	= &RasterizationState,
		.pMultisampleState		= &MultisampleState,
		.pDepthStencilState		= &DepthStencilState,	// Optional
		.pColorBlendState		= &ColorBlendState,
		.pDynamicState			= &DyncmicStateCreateInfo,
		.layout					= Layout,
		.renderPass				= HostRenderPass.GetHandle(),
		.basePipelineHandle		= VK_NULL_HANDLE,		// Optional
		.basePipelineIndex		= -1,					// Optional
	};
		
	VK_CHECK(vkCreateGraphicsPipelines(GVulkan->Device->GetHandle(), GVulkan->RenderPassPipelineCache->GetHandle(), 1, &CreateInfo, GVulkan->AllocationCallbacks, &Handle));
}

void VulkanRenderPass::Subpass::
Destroy() noexcept
{
	vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
	vkDestroyPipelineLayout(GVulkan->Device->GetHandle(), Layout, GVulkan->AllocationCallbacks);
	Layout = VK_NULL_HANDLE;
}
	
VISERA_MODULE_END