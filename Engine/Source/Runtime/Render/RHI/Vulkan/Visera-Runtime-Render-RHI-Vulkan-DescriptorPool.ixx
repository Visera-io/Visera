module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:DescriptorPool;

import :Enums;
import :Device;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanDescriptorPool
	{
	public:
		auto CreateDescriptorSet();

		struct FDescriptorEntry
		{
			EDescriptorType Type;
			UInt32			Count = 0;
		};

		// MaxSets is device specified (usually 32), please check it via vulkanCapsViewer
		FVulkanDescriptorPool(const Array<FDescriptorEntry>& _DescriptorEntries, UInt32 _MaxSets = 32);
		FVulkanDescriptorPool() = delete;

	private:
		VkDescriptorPool				 Handle{ VK_NULL_HANDLE };
		HashMap<EDescriptorType, UInt32> DescriptorTable;
	};

	FVulkanDescriptorPool::
	FVulkanDescriptorPool(
		const Array<FDescriptorEntry>& _DescriptorEntries,
		UInt32 _MaxSets/* = 32*/)
	{
		static_assert(sizeof(FDescriptorEntry)  == sizeof(VkDescriptorPoolSize)  &&
			          alignof(FDescriptorEntry) == alignof(VkDescriptorPoolSize) &&
			          sizeof(FDescriptorEntry::Type) == sizeof(VkDescriptorPoolSize::type));

		for (auto& Entry : _DescriptorEntries)
		{
			DescriptorTable[Entry.Type] = Entry.Count;
		}

		VkDescriptorPoolCreateInfo CreateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets		= _MaxSets,
			.poolSizeCount  = UInt32(_DescriptorEntries.size()),
			.pPoolSizes		= reinterpret_cast<const VkDescriptorPoolSize*>(_DescriptorEntries.data())
		};

		if (vkCreateDescriptorPool(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
			throw SRuntimeError("Failed to create the Vulkan Descriptor Pool!");
	}
	
} } // namespace VE::Runtime