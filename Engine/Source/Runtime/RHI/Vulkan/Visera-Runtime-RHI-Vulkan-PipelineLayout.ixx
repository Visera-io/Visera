module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:PipelineLayout;

import :Common;
import :Device;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE
{
	class RHI;

	class FVulkanPipelineLayout
	{
		friend class RHI; // Created by RHI module
	public:
		// PushConstantRange is device-specified (usually 256B limit), check the limit via @vulkanCapsViewer.
		struct FPushConstantRange
		{
			EVulkanShaderStage ShaderStage;
			UInt16       Offset = 0;
			UInt16       Size   = 0;
			operator VkPushConstantRange() const { return VkPushConstantRange{ .stageFlags = AutoCast(ShaderStage), .offset = Offset, .size = Size }; }
		};

		auto GetPushConstantRanges()	const -> const Array<VkPushConstantRange>&   { return PushConstantRanges; }
		auto GetDescriptorSetLayouts()	const -> const Array<VkDescriptorSetLayout>& { return DescriptorSetLayouts; }
		auto GetHandle() const -> const VkPipelineLayout		{ return Handle; }

		FVulkanPipelineLayout() = delete;
		FVulkanPipelineLayout(const Array<FPushConstantRange>& _PushConstantRanges,
							  const Array<SharedPtr<FVulkanDescriptorSetLayout>>& _DescriptorSetLayouts);
		~FVulkanPipelineLayout();
	protected:
		VkPipelineLayout			 Handle { VK_NULL_HANDLE };
		Array<VkPushConstantRange>	 PushConstantRanges;
		Array<VkDescriptorSetLayout> DescriptorSetLayouts;
	};

	FVulkanPipelineLayout::
	FVulkanPipelineLayout(const Array<FPushConstantRange>& _PushConstantRanges,
						  const Array<SharedPtr<FVulkanDescriptorSetLayout>>& _DescriptorSetLayouts)
	{
		PushConstantRanges.resize(_PushConstantRanges.size());
		std::transform(_PushConstantRanges.begin(), _PushConstantRanges.end(),
					   PushConstantRanges.begin(),
					   [](const auto& _PCRange)
					   -> VkPushConstantRange
					   {return VkPushConstantRange(_PCRange);});

		DescriptorSetLayouts.resize(_DescriptorSetLayouts.size());
		std::transform(_DescriptorSetLayouts.begin(), _DescriptorSetLayouts.end(),
					   DescriptorSetLayouts.begin(),
					   [](const auto& _DSLayout)
					   -> VkDescriptorSetLayout
					   {return _DSLayout->GetHandle();});

		VkPipelineLayoutCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.setLayoutCount			= UInt32(DescriptorSetLayouts.size()),
			.pSetLayouts			= DescriptorSetLayouts.empty()? nullptr : DescriptorSetLayouts.data(),
			.pushConstantRangeCount = UInt32(PushConstantRanges.size()),
			.pPushConstantRanges	= PushConstantRanges.empty()? nullptr : PushConstantRanges.data(),
		};

		if(vkCreatePipelineLayout(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Pipeline Layout!"); }
	}

	FVulkanPipelineLayout::
	~FVulkanPipelineLayout()
	{
		if (Handle != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(
				GVulkan->Device->GetHandle(),
				Handle,
				GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

} // namespace VE