module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPipeline;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Pipeline;
import :PipelineLayout;
import :RenderPipelineSetting;
import :PipelineCache;
import :Device;
import :Shader;

import Visera.Core.Signal;
import Visera.Core.Log;

export namespace VE
{
	class FVulkanRenderPass;

	class FVulkanRenderPipeline : public FVulkanPipeline
	{
		friend class FVulkanRenderPass;
	public:
		auto GetSetting() const -> SharedPtr<const FVulkanRenderPipelineSetting>{ return Setting; }

	protected:
		SharedPtr<const FVulkanRenderPipelineSetting> Setting;
		SharedPtr<const FVulkanShader> VertexShader;
		SharedPtr<const FVulkanShader> FragmentShader;

	public:
		FVulkanRenderPipeline() = delete;
		FVulkanRenderPipeline(SharedPtr<const FVulkanPipelineLayout> _Layout,
			                  SharedPtr<const FVulkanRenderPipelineSetting> _Setting,
			                  SharedPtr<const FVulkanShader> _VertexShader,
			                  SharedPtr<const FVulkanShader> _FragmentShader)
			: FVulkanPipeline{EVulkanPipelineBindPoint::Graphics, _Layout},
			  Setting{ std::move(_Setting) },
			  VertexShader{ std::move(_VertexShader) },
			  FragmentShader{ std::move(_FragmentShader) }
		{ VE_ASSERT(Layout != nullptr && Setting != nullptr && VertexShader != nullptr); }
		~FVulkanRenderPipeline() noexcept { Destroy();}

	private:
		void Create(const VkRenderPass _Owner, UInt32 _SubpassIndex);
		void Destroy();
	};

	void FVulkanRenderPipeline::
	Create(const VkRenderPass _Owner, UInt32 _SubpassIndex)
	{
		if(_Owner == VK_NULL_HANDLE)
		{ throw SRuntimeError("Failed to create the Render Pipeline! -- The Render Pass is NULL!"); }

		Array<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfos;

		if (VertexShader && !VertexShader->IsExpired())
		{
			ShaderStageCreateInfos.emplace_back(VkPipelineShaderStageCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0x0,
				.stage = AutoCast(AutoCast(EVulkanShaderStage::Vertex)),
				.module= VertexShader->GetHandle(),
				.pName = VertexShader->GetEntryPoint().data(),
				.pSpecializationInfo = nullptr
			});
		}
		else { throw SRuntimeError("Failed to create the Render Pipeline! -- The vertex shader is required!"); }
		
		if (FragmentShader)
		{ 
			if (FragmentShader->IsExpired())
			{ throw SRuntimeError("Failed to create the Render Pipeline! -- The fragment shader is expired!"); }

			ShaderStageCreateInfos.emplace_back(VkPipelineShaderStageCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0x0,
				.stage = AutoCast(AutoCast(EVulkanShaderStage::Fragment)),
				.module= FragmentShader->GetHandle(),
				.pName = FragmentShader->GetEntryPoint().data(),
				.pSpecializationInfo = nullptr
			});
		}
		else { Log::Warn("Creating a Render Pipeline without a Fragment Shader!"); }
        
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
			.subpass				= _SubpassIndex,
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
		{ throw SRuntimeError("Failed to create Vulkan Render Pipeline!"); }
	}

	void FVulkanRenderPipeline::
	Destroy()
	{
		if (Handle == VK_NULL_HANDLE) { return; }
		
		Layout.reset();
		Setting.reset();

		vkDestroyPipeline(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE