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

	class FVulkanPipelineLayout : public std::enable_shared_from_this<FVulkanPipelineLayout>
	{
		friend class FVulkanPipeline;
	public:
		// PushConstantRange is device-specified (usually 256B limit), check the limit via @vulkanCapsViewer.
		static inline auto
		Create() -> SharedPtr<FVulkanPipelineLayout> { return CreateSharedPtr<FVulkanPipelineLayout>(); }
		auto inline
		AddPushConstantRange(UInt16 _Offset, UInt16 _Size, EVulkanShaderStage _ShaderStages) -> FVulkanPipelineLayout*;
		auto inline
		AddDescriptorSetLayout(SharedPtr<const FVulkanDescriptorSetLayout> _DescriptorSetLayout) -> FVulkanPipelineLayout*;
		auto inline
		Build() -> SharedPtr<FVulkanPipelineLayout>;
		void inline
		Destory();

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


	protected:
		VkPipelineLayout			                       Handle { VK_NULL_HANDLE };
		Array<VkPushConstantRange>	                       PushConstantRanges;
		Array<SharedPtr<const FVulkanDescriptorSetLayout>> DescriptorSetLayouts;
	};

	FVulkanPipelineLayout* FVulkanPipelineLayout::
	AddPushConstantRange(UInt16 _Offset, UInt16 _Size, EVulkanShaderStage _ShaderStages)
	{
		PushConstantRanges.emplace_back(VkPushConstantRange
			{
				.stageFlags = AutoCast(_ShaderStages),
				.offset = _Offset,
				.size   = _Size,
			});
		return this;
	}

	FVulkanPipelineLayout* FVulkanPipelineLayout::
	AddDescriptorSetLayout(SharedPtr<const FVulkanDescriptorSetLayout> _DescriptorSetLayout)
	{
		DescriptorSetLayouts.emplace_back(std::move(_DescriptorSetLayout));
		return this;
	}

	SharedPtr<FVulkanPipelineLayout> FVulkanPipelineLayout::
	Build()
	{
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
			.pushConstantRangeCount = UInt32(PushConstantRanges.size()),
			.pPushConstantRanges	= PushConstantRanges.empty()? nullptr : PushConstantRanges.data(),
		};

		if(vkCreatePipelineLayout(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Pipeline Layout!"); }

		return shared_from_this();
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