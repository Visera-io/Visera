module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:PipelineLayout;

import :Common;
import :Device;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanPipelineLayout
	{
		friend class FVulkan;
	public:
		// PushConstantRange is device-specified (usually 256B limit), check the limit via @vulkanCapsViewer.
		struct FPushConstantRange
		{
			EShaderStage ShaderStage;
			UInt16       Offset = 0;
			UInt16       Size   = 0;
			operator VkPushConstantRange() { return VkPushConstantRange{ .stageFlags = AutoCast(ShaderStage), .offset = Offset, .size = Size }; }
		};

		auto GetHandle()			const -> const VkPipelineLayout		{ return Handle; }
		auto GetPushConstantRange() const -> const FPushConstantRange&	{ return PushConstantRange; }

		FVulkanPipelineLayout();

	protected:
		VkPipelineLayout   Handle { VK_NULL_HANDLE };
		FPushConstantRange PushConstantRange;
	};

	FVulkanPipelineLayout::
	FVulkanPipelineLayout()
	{
		VkPushConstantRange PCRange{ VkPushConstantRange(PushConstantRange) };
		VkPipelineLayoutCreateInfo LayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount			= 0,
			.pSetLayouts			= nullptr,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges	= &PCRange,
		};

		if(vkCreatePipelineLayout(
			GVulkan->Device->GetHandle(),
			&LayoutCreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Pipeline Layout!"); }
	}

} } // namespace VE::Runtime