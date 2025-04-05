module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPipelineSetting;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :Shader;
import :Swapchain;

import Visera.Core.Signal;

export namespace VE
{

	class FVulkanRenderPipeline;

	class FVulkanRenderPipelineSetting : public std::enable_shared_from_this<FVulkanRenderPipelineSetting>
	{
		friend class FVulkanRenderPipeline;
	public:
		static inline auto
		Create() { return CreateSharedPtr<FVulkanRenderPipelineSetting>(); }
		inline FVulkanRenderPipelineSetting*
		SetCullMode(EVulkanCullMode _Mode)  { RasterizationState.cullMode = AutoCast(_Mode); return this; }
		auto inline
		Confirm() -> SharedPtr<FVulkanRenderPipelineSetting> { bConfirmed = True; return shared_from_this(); }
		Bool inline
		IsConfirmed() const { return bConfirmed; }

		struct FVertexInputDescription
		{
			UInt32 Binding   = 0;
			UInt32 Size      = 0;

			struct FAttribute
			{
				UInt32        Location;
				EVulkanFormat Format;
				UInt32        Offset;
			};
			Array<FAttribute> Attributes;

			Bool   bInstance = False;
		};
		void SetVertexInputState(const FVertexInputDescription& _Description);
		
		FVulkanRenderPipelineSetting();
		~FVulkanRenderPipelineSetting() noexcept = default;

	private:
		/*1*/VkPipelineVertexInputStateCreateInfo	VertexInputState;
		/*2*/VkPipelineTessellationStateCreateInfo	TessellationState;
		/*3*/VkPipelineInputAssemblyStateCreateInfo	InputAssemblyState;
		/*4*/VkPipelineViewportStateCreateInfo		ViewportState;
		/*5*/VkPipelineRasterizationStateCreateInfo	RasterizationState;
		/*6*/VkPipelineMultisampleStateCreateInfo	MultisampleState;
		/*7*/VkPipelineDepthStencilStateCreateInfo	DepthStencilState;
		/*8*/VkPipelineColorBlendStateCreateInfo    ColorBlendState;
		/*9*/VkPipelineDynamicStateCreateInfo       DynamicState;

		Array<VkViewport>                           Viewports;
		Array<VkRect2D>                             Scissors;
		Segment<VkVertexInputBindingDescription, 1> VertexInputBindingDescriptions;   //Default(1)
		Array<VkVertexInputAttributeDescription>    VertexInputAttributeDescriptions;
		Array<VkPipelineColorBlendAttachmentState>  ColorBlendAttachments; //Default(1)
		Array<VkDynamicState>					    DynamicStates;

		Bool bConfirmed = False;
	};

	void FVulkanRenderPipelineSetting::
	SetVertexInputState(const FVertexInputDescription& _Description)
	{
		VertexInputBindingDescriptions[0] = VkVertexInputBindingDescription
		{
			.binding   = _Description.Binding,
			.stride    = _Description.Size,
			.inputRate = _Description.bInstance? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX,
		};

		VertexInputAttributeDescriptions.resize(_Description.Attributes.size());
		for (UInt32 Idx = 0; Idx < VertexInputAttributeDescriptions.size(); ++Idx)
		{
			auto& Attribute = _Description.Attributes[Idx];
			VertexInputAttributeDescriptions[Idx] = VkVertexInputAttributeDescription
			{
				.location = Attribute.Location,
				.binding  = _Description.Binding,
				.format   = AutoCast(Attribute.Format),
				.offset   = Attribute.Offset
			};
		}

		VertexInputState = VkPipelineVertexInputStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount	= UInt32(VertexInputBindingDescriptions.size()),
			.pVertexBindingDescriptions		= VertexInputBindingDescriptions.data(),
			.vertexAttributeDescriptionCount= UInt32(VertexInputAttributeDescriptions.size()),
			.pVertexAttributeDescriptions	= VertexInputAttributeDescriptions.data(),
		};
	}
	
	FVulkanRenderPipelineSetting::
	FVulkanRenderPipelineSetting():
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
			.viewportCount	= 0,		// Dynamic Viewport (Setted by the CommandBuffer) [TODO]: Static? and load by commandbuffer
			.pViewports		= nullptr,
			.scissorCount	= 0,		// Dynamic Scissor  (Setted by the CommandBuffer) [TODO]: Static? and load by commandbuffer
			.pScissors		= nullptr,
		} },
		RasterizationState{ VkPipelineRasterizationStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable		= VK_FALSE, // Fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
			.rasterizerDiscardEnable= VK_FALSE, // if VK_TRUE, then geometry never passes through the rasterizer stage
			.polygonMode			= VK_POLYGON_MODE_FILL,
			.cullMode				= AutoCast(EVulkanCullMode::Back),
			.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE, // Align with Unreal Engine. (Note Y-Flip)
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
		} },
		DynamicStates{
	    {
			VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
			VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT
		} }
	{
		ColorBlendState = VkPipelineColorBlendStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable  = VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
			.logicOp        = VK_LOGIC_OP_COPY,
			.attachmentCount= UInt32(ColorBlendAttachments.size()),
			.pAttachments	= ColorBlendAttachments.data(),
			.blendConstants	= { 0.0f, 0.0f, 0.0f, 0.0f },
		};

		DynamicState = VkPipelineDynamicStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = UInt32(DynamicStates.size()),
		    .pDynamicStates    = DynamicStates.data(),
		};
	}

} // namespace VE