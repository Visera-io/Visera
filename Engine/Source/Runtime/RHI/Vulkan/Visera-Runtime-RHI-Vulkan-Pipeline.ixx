module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Pipeline;

import :Common;
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
		
		FVulkanPipeline() = delete;
		FVulkanPipeline(EVulkanPipelineBindPoint _BindPoint, SharedPtr<const FVulkanPipelineLayout> _Layout) 
			:BindPoint{_BindPoint}, Layout{ std::move(_Layout) } {}
		
	protected:
		VkPipeline								Handle{ VK_NULL_HANDLE };
		EVulkanPipelineBindPoint				BindPoint;
		SharedPtr<const FVulkanPipelineLayout>	Layout;
	};

} // namespace VE