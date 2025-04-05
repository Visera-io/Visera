module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:PipelineLayout;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE
{
	class FVulkanPipeline;

	class FVulkanPipelineLayout
	{
		friend class FVulkanPipeline;
	public:
		// PushConstantRange is device-specified (usually 256B limit), check the limit via @vulkanCapsViewer.
		struct FPushConstantRange
		{
			EVulkanShaderStage ShaderStages;
			UInt16       Offset = 0;
			UInt16       Size   = 0;
			operator VkPushConstantRange() const { return VkPushConstantRange{ .stageFlags = AutoCast(ShaderStages), .offset = Offset, .size = Size }; }
		};
		static inline auto
		Create() -> SharedPtr<FVulkanPipelineLayout> { return CreateSharedPtr<FVulkanPipelineLayout>(); }

		auto inline
		AddPushConstantRange(const FPushConstantRange& _PushConstantRange) -> FVulkanPipelineLayout*;
		auto inline
		AddDescriptorSetLayout(SharedPtr<const FVulkanDescriptorSetLayout> _DescriptorSetLayout) -> FVulkanPipelineLayout*;

		auto inline
		GetHandle() const -> const VkPipelineLayout		{ return Handle; }

		Bool inline
		HasBuilt() const { return Handle != VK_NULL_HANDLE; }
		Bool inline
		HasPushConstant() const { return !PushConstantRanges.empty(); }
		Bool inline
		HasDescriptorSetLayout() const { return !DescriptorSetLayouts.empty(); }

		FVulkanPipelineLayout();
		~FVulkanPipelineLayout();

	private:
		void inline Build();
		void inline Destory();

	protected:
		VkPipelineLayout			                       Handle { VK_NULL_HANDLE };
		Array<FPushConstantRange>	                       PushConstantRanges;
		Array<SharedPtr<const FVulkanDescriptorSetLayout>> DescriptorSetLayouts;
	};

	FVulkanPipelineLayout* FVulkanPipelineLayout::
	AddPushConstantRange(const FPushConstantRange& _PushConstantRange)
	{
		PushConstantRanges.emplace_back(_PushConstantRange);
		return this;
	}

	FVulkanPipelineLayout* FVulkanPipelineLayout::
	AddDescriptorSetLayout(SharedPtr<const FVulkanDescriptorSetLayout> _DescriptorSetLayout)
	{
		DescriptorSetLayouts.emplace_back(std::move(_DescriptorSetLayout));
		return this;
	}

	void FVulkanPipelineLayout::
	Build()
	{
		Array<VkPushConstantRange> PushConstantRangeInfos(PushConstantRanges.size());
		std::transform(PushConstantRanges.begin(), PushConstantRanges.end(),
					   PushConstantRangeInfos.begin(),
					   [](const auto& _PCRange)
					   -> VkPushConstantRange
					   {return VkPushConstantRange(_PCRange);});

		Array<VkDescriptorSetLayout> DescriptorSetLayoutInfos(DescriptorSetLayouts.size());
		std::transform(DescriptorSetLayouts.begin(), DescriptorSetLayouts.end(),
					   DescriptorSetLayoutInfos.begin(),
					   [](const auto& _DSLayout)
					   -> VkDescriptorSetLayout
					   {return _DSLayout->GetHandle();});

		VkPipelineLayoutCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.setLayoutCount			= UInt32(DescriptorSetLayoutInfos.size()),
			.pSetLayouts			= DescriptorSetLayoutInfos.empty()? nullptr : DescriptorSetLayoutInfos.data(),
			.pushConstantRangeCount = UInt32(PushConstantRangeInfos.size()),
			.pPushConstantRanges	= PushConstantRangeInfos.empty()? nullptr : PushConstantRangeInfos.data(),
		};

		if(vkCreatePipelineLayout(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Pipeline Layout!"); }
	}

	void FVulkanPipelineLayout::
	Destory()
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

	FVulkanPipelineLayout::
	FVulkanPipelineLayout()
	{
		
	}

	FVulkanPipelineLayout::
	~FVulkanPipelineLayout()
	{
		Destory();
	}

} // namespace VE