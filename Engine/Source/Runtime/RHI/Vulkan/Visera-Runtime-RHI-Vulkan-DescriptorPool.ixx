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
	class RHI;

	class FVulkanDescriptorPool : public std::enable_shared_from_this<FVulkanDescriptorPool>
	{
		friend class RHI;
	public:
		// Return nullptr when exceeded MaxSets Limit.
		auto CreateDescriptorSet(SharedPtr<FVulkanDescriptorSetLayout> _Layout) -> SharedPtr<FVulkanDescriptorSet>;

		struct FDescriptorEntry
		{
			EDescriptorType Type;
			UInt32			Count = 0;
		};

		FVulkanDescriptorPool() noexcept = default;
		~FVulkanDescriptorPool() = default;
	private:
		VkDescriptorPool						Handle{ VK_NULL_HANDLE };
		UInt32									MaxSets{ 0 };
		HashMap<EDescriptorType, Int32>			DescriptorTable;
		List<SharedPtr<FVulkanDescriptorSet>>	Children;

	private:
		void Create(const Array<FDescriptorEntry>& _DescriptorEntries, UInt32 _MaxSets);
		void Destroy();
	};

	void FVulkanDescriptorPool::
	Create(const Array<FDescriptorEntry>&_DescriptorEntries, UInt32 _MaxSets)
	{
		static_assert(sizeof(FDescriptorEntry)  == sizeof(VkDescriptorPoolSize)  &&
			          alignof(FDescriptorEntry) == alignof(VkDescriptorPoolSize) &&
			          sizeof(FDescriptorEntry::Type) == sizeof(VkDescriptorPoolSize::type));

		MaxSets = _MaxSets; VE_ASSERT(MaxSets > 0);

		for (auto& Entry : _DescriptorEntries)
		{
			DescriptorTable[Entry.Type] += Entry.Count;
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

	void FVulkanDescriptorPool::
	Destroy()
	{
		vkDestroyDescriptorPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	SharedPtr<FVulkanDescriptorSet> FVulkanDescriptorPool::
	CreateDescriptorSet(SharedPtr<FVulkanDescriptorSetLayout> _Layout)
	{
		if (Children.size() >= MaxSets)
		{ throw SRuntimeError(Text("Cannot create more DescriptorSet from this DescriptorPool! -- (MaxSets:{})", MaxSets)); }

		for (auto& Binding : _Layout->BindingSlots)
		{
			auto& Resource = DescriptorTable[Binding.DescriptorType];
			Resource -= Binding.DescriptorCount;
			if (Resource < 0)
			{ throw SRuntimeError(Text("Failed to allocate Descriptor({}) from current DescriptorPool!", UInt32(Binding.DescriptorType))); }
		}

		auto DescriptorSet = CreateSharedPtr<FVulkanDescriptorSet>(shared_from_this(), _Layout);

		VkDescriptorSetAllocateInfo AllocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool		= Handle,
			.descriptorSetCount = 1,
			.pSetLayouts		= &_Layout->Handle,
		};

		if (vkAllocateDescriptorSets(
			GVulkan->Device->GetHandle(),
			&AllocateInfo,
			&DescriptorSet->Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create the Vulkan Descriptor Sets!"); }
		
		return Children.emplace_back(std::move(DescriptorSet));
	}
	
} } // namespace VE::Runtime