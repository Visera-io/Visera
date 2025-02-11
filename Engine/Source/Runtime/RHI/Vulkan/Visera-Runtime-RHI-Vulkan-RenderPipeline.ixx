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
		SharedPtr<FVulkanRenderPipelineLayout>	Layout;

	public:
		FVulkanRenderPipeline() = delete;
		FVulkanRenderPipeline(SharedPtr<FVulkanRenderPipelineLayout> _Layout) : Layout{ std::move(_Layout) } { VE_ASSERT(Layout != nullptr); }
		~FVulkanRenderPipeline() noexcept { Destroy();}

	private:
		void Create(const VkRenderPass _Owner, SharedPtr<const FVulkanShader> _VertexShader, SharedPtr<const FVulkanShader> _FragmentShader);
		void Destroy();
	};

	void FVulkanRenderPipeline::
	Create(const VkRenderPass _Owner,
		   SharedPtr<const FVulkanShader> _VertexShader,
		   SharedPtr<const FVulkanShader> _FragmentShader)
	{
		VE_ASSERT(_Owner != VK_NULL_HANDLE   &&
				  _VertexShader != nullptr   && (Bool(_VertexShader->GetStage() & EVulkanShaderStage::Vertex)) &&
				  _FragmentShader != nullptr && (Bool(_FragmentShader->GetStage() & EVulkanShaderStage::Fragment)));

		auto ShaderStageCreateInfos = Segment<VkPipelineShaderStageCreateInfo, MaxShaderSlot>{};
		ShaderStageCreateInfos[Vertex] = VkPipelineShaderStageCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.stage = AutoCast(AutoCast(EVulkanShaderStage::Vertex)),
			.module= _VertexShader->GetHandle(),
			.pName = _VertexShader->GetEntryPoint().data(),
			.pSpecializationInfo = nullptr
		};

		ShaderStageCreateInfos[Fragment] = VkPipelineShaderStageCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.stage = AutoCast(AutoCast(EVulkanShaderStage::Fragment)),
			.module= _FragmentShader->GetHandle(),
			.pName = _FragmentShader->GetEntryPoint().data(),
			.pSpecializationInfo = nullptr
		};
        
		VkGraphicsPipelineCreateInfo CreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.flags = 0x0,
			.stageCount				= UInt32(ShaderStageCreateInfos.size()),
			.pStages				= ShaderStageCreateInfos.data(),
			.pVertexInputState		= &Layout->VertexInputState,
			.pInputAssemblyState	= &Layout->InputAssemblyState,
			.pViewportState			= &Layout->GetViewportState(),
			.pRasterizationState	= &Layout->RasterizationState,
			.pMultisampleState		= &Layout->MultisampleState,
			.pDepthStencilState		= &Layout->DepthStencilState,	// Optional
			.pColorBlendState		= &Layout->GetColorBlendState(),
			.pDynamicState			= &Layout->GetDynamicStates(),
			.layout					= Layout->GetHandle(),
			.renderPass				= _Owner,
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