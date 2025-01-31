module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSet;

import :Enums;
import :Device;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanDescriptorSet
	{
	public:

		FVulkanDescriptorSet() = delete;
		FVulkanDescriptorSet(VkDescriptorPool _Owner, SharedPtr<FVulkanDescriptorSetLayout> _Layout);
		~FVulkanDescriptorSet() = default; // Recollected via its owner automatically.
	private:
		VkDescriptorSet  Handle{ VK_NULL_HANDLE };
		VkDescriptorPool Owner { VK_NULL_HANDLE };
		SharedPtr<FVulkanDescriptorSetLayout> Layout;
	};

	FVulkanDescriptorSet::
	FVulkanDescriptorSet(VkDescriptorPool _Owner, SharedPtr<FVulkanDescriptorSetLayout> _Layout)
		: Owner{ _Owner },
		  Layout { std::move(_Layout) }
	{
		auto LayoutHandle = Layout->GetHandle();

		VkDescriptorSetAllocateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool		= Owner,
			.descriptorSetCount = 1,
			.pSetLayouts		= &LayoutHandle,
		};

		if (vkAllocateDescriptorSets(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create the Vulkan Descriptor Sets!"); }
	}
	
} } // namespace VE::Runtime