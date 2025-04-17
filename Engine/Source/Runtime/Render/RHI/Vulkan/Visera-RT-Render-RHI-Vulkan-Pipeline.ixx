module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Pipeline;
#define VE_MODULE_NAME "Vulkan:Pipeline"
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :PipelineLayout;

import Visera.Core.Log;

export namespace VE
{

	class FVulkanPipeline
	{
	public:
		auto GetLayout()	const -> SharedPtr<const FVulkanPipelineLayout>		{ return Layout; }
		auto GetBindPoint() const -> EVulkanPipelineBindPoint					{ return BindPoint;}
		auto GetHandle()	const -> const VkPipeline							{ return Handle; }

		Bool HasPushConstant() const { return Layout->HasPushConstant(); }
		
		FVulkanPipeline() = delete;
		FVulkanPipeline(EVulkanPipelineBindPoint _BindPoint, SharedPtr<const FVulkanPipelineLayout> _Layout) 
			:BindPoint{_BindPoint}, Layout{ std::move(_Layout) }
		{ if(!Layout->IsBuilt()) { VE_LOG_FATAL("For logic safty, you MUST built PipelineLayout in advance!"); } }
		
	protected:
		VkPipeline                             Handle{ VK_NULL_HANDLE };
		EVulkanPipelineBindPoint               BindPoint;
		SharedPtr<const FVulkanPipelineLayout> Layout;
	};

} // namespace VE