module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderPipeline;
#define VE_MODULE_NAME "Vulkan:RenderPipeline"
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Pipeline;
import :PipelineLayout;
import :RenderPipelineSetting;
import :PipelineCache;
import :Device;
import :Shader;

import Visera.Core.Log;

export namespace VE
{
	class FVulkanRenderPass;

	class FVulkanRenderPipeline : public FVulkanPipeline
	{
		friend class FVulkanRenderPass;
	public:
		static inline auto
		Create(SharedPtr<const FVulkanPipelineLayout>        _Layout,
			   SharedPtr<const FVulkanRenderPipelineSetting> _Setting,
			   SharedPtr<const FVulkanSPIRVShader>           _VertexShader,
			   SharedPtr<const FVulkanSPIRVShader>           _FragmentShader)
		{ return CreateSharedPtr<FVulkanRenderPipeline>(_Layout, _Setting, _VertexShader, _FragmentShader); }

		auto GetSetting() const -> SharedPtr<const FVulkanRenderPipelineSetting>{ return Setting; }

	protected:
		SharedPtr<const FVulkanRenderPipelineSetting> Setting;
		SharedPtr<const FVulkanSPIRVShader> VertexShader;
		SharedPtr<const FVulkanSPIRVShader> FragmentShader;

	public:
		FVulkanRenderPipeline() = delete;
		FVulkanRenderPipeline(SharedPtr<const FVulkanPipelineLayout>        _Layout,
			                  SharedPtr<const FVulkanRenderPipelineSetting> _Setting,
			                  SharedPtr<const FVulkanSPIRVShader>           _VertexShader,
			                  SharedPtr<const FVulkanSPIRVShader>           _FragmentShader)
			: FVulkanPipeline{EVulkanPipelineBindPoint::Graphics, _Layout},
			  Setting{ std::move(_Setting) },
			  VertexShader{ std::move(_VertexShader) },
			  FragmentShader{ std::move(_FragmentShader) }
		{ VE_ASSERT(Layout != nullptr && Setting != nullptr && VertexShader != nullptr); }
		~FVulkanRenderPipeline() noexcept { Destroy();}

	private:
		void Build(const VkRenderPass _Owner, UInt32 _SubpassIndex);
		void Destroy();
	};

	void FVulkanRenderPipeline::
	Build(const VkRenderPass _Owner, UInt32 _SubpassIndex)
	{
		if(_Owner == VK_NULL_HANDLE)
		{ VE_LOG_FATAL("Failed to build the Render Pipeline! -- The Render Pass is NULL!"); }

		if(!Layout->IsBuilt())
		{ VE_LOG_FATAL("Failed to build RenderPipeline! -- The Layout is not built!"); }

		if(!Setting->IsConfirmed())
		{ VE_LOG_FATAL("Failed to build RenderPipeline! -- The Setting is not confirmed!"); }
		
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
		else { VE_LOG_FATAL("Failed to create the Render Pipeline! -- The vertex shader is required!"); }
		
		if (FragmentShader)
		{ 
			if (FragmentShader->IsExpired())
			{ VE_LOG_FATAL("Failed to create the Render Pipeline! -- The fragment shader is expired!"); }

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
		else { VE_LOG_WARN("Creating a Render Pipeline without a Fragment Shader!"); }
        
		VkGraphicsPipelineCreateInfo CreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.flags = 0x0,
			.stageCount				= UInt32(ShaderStageCreateInfos.size()),
			.pStages				= ShaderStageCreateInfos.data(),
			.pVertexInputState		= &Setting->VertexInputState,
			.pInputAssemblyState	= &Setting->InputAssemblyState,
			.pViewportState			= &Setting->ViewportState,
			.pRasterizationState	= &Setting->RasterizationState,
			.pMultisampleState		= &Setting->MultisampleState,
			.pDepthStencilState		= &Setting->DepthStencilState,	// Optional
			.pColorBlendState		= &Setting->ColorBlendState,
			.pDynamicState			= &Setting->DynamicState,
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
		{ VE_LOG_FATAL("Failed to create Vulkan Render Pipeline!"); }
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