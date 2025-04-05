module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Pipeline;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :PipelineLayout;

import Visera.Core.Signal;

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
		{ if(!Layout->HasBuilt()) { std::const_pointer_cast<FVulkanPipelineLayout>(Layout)->Build(); } }
		
	protected:
		VkPipeline                             Handle{ VK_NULL_HANDLE };
		EVulkanPipelineBindPoint               BindPoint;
		SharedPtr<const FVulkanPipelineLayout> Layout;
	};

} // namespace VE