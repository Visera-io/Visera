module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPipeline;

import :Enums;
import :Device;
import :Shader;
import :Swapchain;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanRenderPipeline
	{
		friend class FVulkanRenderPass;
	public:
		auto GetShaderStages()		const -> const Array<VkPipelineShaderStageCreateInfo>& { return ShaderStages; }
		auto GetVertexInputState()	const -> const VkPipelineVertexInputStateCreateInfo&   { return VertexInputState; }
		auto GetTessellationState()	const -> const VkPipelineTessellationStateCreateInfo&  { return TessellationState; }
		auto GetInputAssemblyState()const -> const VkPipelineInputAssemblyStateCreateInfo& { return InputAssemblyState; }
		auto GetViewports()			const -> const Array<VkViewport>&					   { return Viewports; }
		auto GetScissors()			const -> const Array<VkRect2D>&						   { return Scissors;  }
		auto GetRasterizationState()const -> const VkPipelineRasterizationStateCreateInfo& { return RasterizationState; }
		auto GetMultisampleState()	const -> const VkPipelineMultisampleStateCreateInfo&   { return MultisampleState; }
		auto GetDepthStencilState() const -> const VkPipelineDepthStencilStateCreateInfo&  { return DepthStencilState; };
		
		auto GetColorBlendAttachments() const -> const Array<VkPipelineColorBlendAttachmentState>& { return ColorBlendAttachments; }
		auto GetDynamicStates()			const -> const Array<VkDynamicState>& { return DynamicStates; };

	protected:
		VkPipeline									Handle{ VK_NULL_HANDLE };

		Array<VkPipelineShaderStageCreateInfo>		ShaderStages;

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
		FVulkanRenderPipeline();
		~FVulkanRenderPipeline() noexcept { }
	};

		
	FVulkanRenderPipeline::
	FVulkanRenderPipeline():
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
		/*Viewports{ {
			.x		= 0.0f, 
			.y		=	Float(GVulkan->Swapchain->GetExtent().height),
			.width	=	Float(GVulkan->Swapchain->GetExtent().width),
			.height	= - Float(GVulkan->Swapchain->GetExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,} },*/
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
		VE_WIP;
	}

} } // namespace VE::Runtime