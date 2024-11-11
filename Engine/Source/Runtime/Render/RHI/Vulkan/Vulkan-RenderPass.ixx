module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:RenderPass;

import :Context;
import :Allocator;
import :Device;
import :Shader;
import :CommandPool;
import :Swapchain;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanRenderPass
	{
		friend class Vulkan;
		class Subpass;
	public:
		virtual void Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const;
		virtual void Stop(SharedPtr<VulkanCommandPool::CommandBuffer>  CommandBuffer) const;
		auto GetSubpasses() const -> const Array<Subpass>&	{ return Subpasses; }
		auto GetHandle()	const -> VkRenderPass			{ return Handle; }
		operator VkRenderPass()	const { return Handle; }
		
	public:
		class Subpass final
		{
		public:
			void Create(const VulkanRenderPass& HostRenderPass, const Array<SharedPtr<VulkanShader>>& Shaders);
			void Destroy() noexcept;

			auto GetLayout()	const  ->	VkPipelineLayout			{ return Layout; }
			auto GetViewports() const  ->	const Array<VkViewport>&	{ return Viewports; }
			auto GetScissors()  const  ->	const Array<VkRect2D>&		{ return Scissors;  }
			auto GetColorBlendAttachments() const -> const Array<VkPipelineColorBlendAttachmentState>& { return ColorBlendAttachments; }
			auto GetHandle()		const  ->	VkPipeline { return Handle; }
			operator VkPipeline()	const  { return Handle; }

		private:
			VkPipeline				Handle { VK_NULL_HANDLE };
			VkPipelineLayout		Layout { VK_NULL_HANDLE };

			Array<VkPipelineShaderStageCreateInfo>	ShaderStages;
			Array<VkViewport>						Viewports;
			Array<VkRect2D>							Scissors;

			VkPipelineVertexInputStateCreateInfo	VertexInputState;
			VkPipelineTessellationStateCreateInfo	TessellationState;
			VkPipelineInputAssemblyStateCreateInfo	InputAssemblyState;
			VkPipelineViewportStateCreateInfo		ViewportState;
			VkPipelineRasterizationStateCreateInfo	RasterizationState;
			VkPipelineMultisampleStateCreateInfo	MultisampleState;
			VkPipelineDepthStencilStateCreateInfo	DepthStencilState;
			VkPipelineColorBlendStateCreateInfo		ColorBlendState;
			VkPipelineDynamicStateCreateInfo		DynamicState;
			Array<VkPipelineColorBlendAttachmentState> ColorBlendAttachments;

		public:
			Subpass();
			~Subpass() noexcept { Destroy(); }
		};

	protected:
		struct FrameBuffer
		{
			VkFramebuffer		Handle{ VK_NULL_HANDLE };
			Array<VkImageView>	RenderTargets;
			Array<VkClearValue>	ClearColors;
			operator VkFramebuffer() const { return Handle; }
			~FrameBuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks); Handle = VK_NULL_HANDLE; }
		};
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		UInt32							Priority { 0 }; // Less is More
		VkRect2D						RenderArea; //Default: {{0,0}, {Swapchain.Extent}}
		Array<FrameBuffer>				FrameBuffers;
		Array<VkAttachmentDescription>  Attachments;
		Array<Subpass>					Subpasses;

	public:
		VulkanRenderPass();
		virtual ~VulkanRenderPass() noexcept;
	};

	void VulkanRenderPass::
	Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const
	{
		Assert(CommandBuffer->IsRecording());

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
		Assert(CommandBuffer->IsRecording());
		vkCmdEndRenderPass(CommandBuffer->GetHandle());
	}

	VulkanRenderPass::
	VulkanRenderPass()
		:RenderArea{{0,0}, { GVulkan->Swapchain->GetExtent() }}
	{
		FrameBuffers.resize(GVulkan->Swapchain->GetImages().size());
	}

	VulkanRenderPass::
	~VulkanRenderPass() noexcept
	{
		Subpasses.clear();
		FrameBuffers.clear();
		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
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
		ViewportState{ VkPipelineViewportStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount	= UInt32(Viewports.size()),
			.pViewports		= Viewports.data(),
			.scissorCount	= UInt32(Scissors.size()),
			.pScissors		= Scissors.data(),
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
		ColorBlendState{ VkPipelineColorBlendStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable		= VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
			.logicOp			= VK_LOGIC_OP_COPY,
			.attachmentCount	= UInt32(ColorBlendAttachments.size()),
			.pAttachments		= ColorBlendAttachments.data(),
			.blendConstants		= {0.0f, 0.0f, 0.0f, 0.0f}
		} },
		DynamicState{ VkPipelineDynamicStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 0,
		}}
	{
		//!!!Remeber to call Subpass::Create() in Renderpass!!!

		//[TODO][FIXME]: Add SPIR-V Reflection?
		VkPipelineLayoutCreateInfo LayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount			= 0,
			.pSetLayouts			= nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges	= nullptr,
		};
		VK_CHECK(vkCreatePipelineLayout(GVulkan->Device->GetHandle(), &LayoutCreateInfo, VulkanAllocator::AllocationCallbacks, &Layout));
	}

	void VulkanRenderPass::Subpass::
	Create(const VulkanRenderPass& HostRenderPass, const Array<SharedPtr<VulkanShader>>& Shaders)
	{
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

		VkGraphicsPipelineCreateInfo CreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount				= 2,
			.pStages				= ShaderStages.data(),
			.pVertexInputState		= &VertexInputState,
			.pInputAssemblyState	= &InputAssemblyState,
			.pViewportState			= &ViewportState,
			.pRasterizationState	= &RasterizationState,
			.pMultisampleState		= &MultisampleState,
			.pDepthStencilState		= &DepthStencilState,	// Optional
			.pColorBlendState		= &ColorBlendState,
			.pDynamicState			= &DynamicState,
			.layout					= Layout,
			.renderPass				= HostRenderPass.GetHandle(),
			.basePipelineHandle		= VK_NULL_HANDLE,		// Optional
			.basePipelineIndex		= -1,					// Optional
		};

		//vkCreateGraphicsPipelines(GVulkan->Device->GetHandle(), )
	}

	void VulkanRenderPass::Subpass::
	Destroy() noexcept
	{
		vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
	
} } // namespace VE::Runtime