module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPipeline;

import :PipelineLayout;
import :RenderPipelineSetting;
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
		SharedPtr<FVulkanPipelineLayout>		Layout;
		SharedPtr<FVulkanRenderPipelineSetting>	Setting;

	public:
		FVulkanRenderPipeline() = delete;
		FVulkanRenderPipeline(SharedPtr<FVulkanPipelineLayout> _Layout, SharedPtr<FVulkanRenderPipelineSetting> _Setting) 
			: Layout{std::move(_Layout)}, Setting{ std::move(_Setting) } { VE_ASSERT(Layout != nullptr && Setting != nullptr); }
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
			.pVertexInputState		= &Setting->VertexInputState,
			.pInputAssemblyState	= &Setting->InputAssemblyState,
			.pViewportState			= &Setting->GetViewportState(),
			.pRasterizationState	= &Setting->RasterizationState,
			.pMultisampleState		= &Setting->MultisampleState,
			.pDepthStencilState		= &Setting->DepthStencilState,	// Optional
			.pColorBlendState		= &Setting->GetColorBlendState(),
			.pDynamicState			= &Setting->GetDynamicStates(),
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