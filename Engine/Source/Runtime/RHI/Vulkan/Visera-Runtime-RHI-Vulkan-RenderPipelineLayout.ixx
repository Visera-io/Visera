module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPipelineLayout;

import :Common;
import :Device;
import :Shader;
import :Swapchain;
import :PipelineLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPipeline;

	class FVulkanRenderPipelineLayout : public FVulkanPipelineLayout
	{
		friend class FVulkanRenderPipeline;
	public:
		auto GetVertexInputState()	const -> const VkPipelineVertexInputStateCreateInfo&   { return VertexInputState; }
		auto GetTessellationState()	const -> const VkPipelineTessellationStateCreateInfo&  { return TessellationState; }
		auto GetInputAssemblyState()const -> const VkPipelineInputAssemblyStateCreateInfo& { return InputAssemblyState; }
		auto GetViewportState()		const -> const VkPipelineViewportStateCreateInfo&	   { return ViewportState; }
		auto GetRasterizationState()const -> const VkPipelineRasterizationStateCreateInfo& { return RasterizationState; }
		auto GetMultisampleState()	const -> const VkPipelineMultisampleStateCreateInfo&   { return MultisampleState; }
		auto GetDepthStencilState() const -> const VkPipelineDepthStencilStateCreateInfo&  { return DepthStencilState; };
		
		auto GetColorBlendState()	const -> const VkPipelineColorBlendStateCreateInfo&;
		auto GetDynamicStates()		const -> const VkPipelineDynamicStateCreateInfo&;

	protected:
		/*1*/VkPipelineVertexInputStateCreateInfo	VertexInputState;
		/*2*/VkPipelineTessellationStateCreateInfo	TessellationState;
		/*3*/VkPipelineInputAssemblyStateCreateInfo	InputAssemblyState;
		/*4*/VkPipelineViewportStateCreateInfo		ViewportState;
		/*5*/VkPipelineRasterizationStateCreateInfo	RasterizationState;
		/*6*/VkPipelineMultisampleStateCreateInfo	MultisampleState;
		/*7*/VkPipelineDepthStencilStateCreateInfo	DepthStencilState;
		/*8*/Array<VkPipelineColorBlendAttachmentState> ColorBlendAttachments; //Default(1)
		/*9*/Array<VkDynamicState>					DynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		
	public:
		FVulkanRenderPipelineLayout() = delete;
		FVulkanRenderPipelineLayout(const Array<FPushConstantRange>& _PushConstantRanges,
									const Array<SharedPtr<FVulkanDescriptorSetLayout>>& _DescriptorSetLayouts);
		~FVulkanRenderPipelineLayout() noexcept = default;
	};

		
	FVulkanRenderPipelineLayout::
	FVulkanRenderPipelineLayout(
		const Array<FPushConstantRange>& _PushConstantRanges,
		const Array<SharedPtr<FVulkanDescriptorSetLayout>>& _DescriptorSetLayouts)
		:
		FVulkanPipelineLayout
		{ 
			_PushConstantRanges, 
			_DescriptorSetLayouts
		},
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
			.sType		= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		} },
		ViewportState{ VkPipelineViewportStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount	= 0,		// Dynamic Viewport (Setted by the CommandBuffer)
			.pViewports		= nullptr,
			.scissorCount	= 0,		// Dynamic Scissor  (Setted by the CommandBuffer)
			.pScissors		= nullptr,
		} },
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
		
	}

	const VkPipelineColorBlendStateCreateInfo& FVulkanRenderPipelineLayout::
	GetColorBlendState() const
	{
		static VkPipelineColorBlendStateCreateInfo ColorBlendState
		{ 
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
			.logicOp = VK_LOGIC_OP_COPY,
			.blendConstants	= { 0.0f, 0.0f, 0.0f, 0.0f },
		};
		ColorBlendState.attachmentCount = UInt32(ColorBlendAttachments.size());
		ColorBlendState.pAttachments	= ColorBlendAttachments.data();

		return ColorBlendState;
	}

	const VkPipelineDynamicStateCreateInfo& FVulkanRenderPipelineLayout::
	GetDynamicStates() const
	{
		static VkPipelineDynamicStateCreateInfo DyncmicStateCreateInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

		DyncmicStateCreateInfo.dynamicStateCount = UInt32(DynamicStates.size());
		DyncmicStateCreateInfo.pDynamicStates    = DynamicStates.data();

		return DyncmicStateCreateInfo;
	}

} } // namespace VE::Runtime