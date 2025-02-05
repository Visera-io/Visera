module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPipeline;

import :RenderPipelineLayout;
import :PipelineCache;
import :Device;
import :Shader;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanRenderPipeline
	{
		friend class FVulkanRenderPass;
	public:
		enum EShaderSlot { Vertex, Fragment, MaxShaderSlot };

	protected:
		VkPipeline								Handle{ VK_NULL_HANDLE };
		const VkRenderPass const				Owner;
		SharedPtr<FVulkanRenderPipelineLayout>	Layout;

		Segment<FVulkanShader, EShaderSlot::MaxShaderSlot> Shaders;

	public:
		FVulkanRenderPipeline() = delete;
		FVulkanRenderPipeline(const VkRenderPass const _Owner) : Owner{ _Owner } { }
		~FVulkanRenderPipeline() noexcept { Destroy();}

	private:
		void Create();
		void Destroy();
	};

	void FVulkanRenderPipeline::
	Create()
	{
		VE_ASSERT(Owner != VK_NULL_HANDLE);

		auto ShaderStageCreateInfo = Segment<VkPipelineShaderStageCreateInfo, EShaderSlot::MaxShaderSlot>
		{

		};
		VE_WIP;

		VkGraphicsPipelineCreateInfo CreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.flags = 0x0,
			.stageCount				= UInt32(ShaderStageCreateInfo.size()),
			.pStages				= ShaderStageCreateInfo.data(),
			.pVertexInputState		= &Layout->VertexInputState,
			.pInputAssemblyState	= &Layout->InputAssemblyState,
			.pViewportState			= &Layout->GetViewportState(),
			.pRasterizationState	= &Layout->RasterizationState,
			.pMultisampleState		= &Layout->MultisampleState,
			.pDepthStencilState		= &Layout->DepthStencilState,	// Optional
			.pColorBlendState		= &Layout->GetColorBlendState(),
			.pDynamicState			= &Layout->GetDynamicStates(),
			.layout					= Layout->GetHandle(),
			.renderPass				= Owner,
			.basePipelineHandle		= VK_NULL_HANDLE,		// Optional
			.basePipelineIndex		= -1,					// Optional
		};
		
		if(vkCreateGraphicsPipelines(
			GVulkan->Device->GetHandle(),
			GVulkan->GraphicsPipelineCache->GetHandle(),
			1,
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create one Subpass in current RenderPass!"); }
	}

	void FVulkanRenderPipeline::
	Destroy()
	{
		if (Handle == VK_NULL_HANDLE) { return; }
		
		vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime