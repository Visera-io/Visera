module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorPool;

import :Enums;
import :Device;
import :GPU;
import :DescriptorSet;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanDescriptorPool
	{
	public:
		// Return nullptr when exceeded MaxSets Limit.
		auto CreateDescriptorSet(SharedPtr<FVulkanDescriptorSetLayout> _Layout) -> SharedPtr<FVulkanDescriptorSet> { static UInt32 SetCount = 0; if (++SetCount > MaxSets) { return nullptr; } else { return CreateSharedPtr<FVulkanDescriptorSet>(Handle, _Layout); } }

		struct FDescriptorEntry
		{
			EDescriptorType Type;
			UInt32			Count = 0;
		};

		FVulkanDescriptorPool(const Array<FDescriptorEntry>& _DescriptorEntries, UInt32 _MaxSets);
		FVulkanDescriptorPool() = delete;
		~FVulkanDescriptorPool();
	private:
		VkDescriptorPool				 Handle{ VK_NULL_HANDLE };
		UInt32							 MaxSets{ 0 };
		HashMap<EDescriptorType, UInt32> DescriptorTable;
	};

	FVulkanDescriptorPool::
	FVulkanDescriptorPool(
		const Array<FDescriptorEntry>& _DescriptorEntries,
		UInt32 _MaxSets/* = GVulkan->GPU->GetProperties().limits.maxBoundDescriptorSets */)
		: MaxSets{ _MaxSets }
	{
		static_assert(sizeof(FDescriptorEntry)  == sizeof(VkDescriptorPoolSize)  &&
			          alignof(FDescriptorEntry) == alignof(VkDescriptorPoolSize) &&
			          sizeof(FDescriptorEntry::Type) == sizeof(VkDescriptorPoolSize::type));
		VE_ASSERT(MaxSets > 0);

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
		{ throw SRuntimeError("Failed to create the Vulkan Descriptor Pool!"); }
	}

	FVulkanDescriptorPool::
	~FVulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
	
} } // namespace VE::Runtime