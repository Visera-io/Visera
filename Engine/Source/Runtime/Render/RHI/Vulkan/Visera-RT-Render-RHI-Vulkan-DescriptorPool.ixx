module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:DescriptorPool;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :DescriptorSet;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE
{

	class FVulkanDescriptorPool : public std::enable_shared_from_this<FVulkanDescriptorPool>
	{
	public:
		static inline auto
		Create() { return CreateSharedPtr<FVulkanDescriptorPool>(); }

		// Return nullptr when exceeded MaxSets Limit.
		auto CreateDescriptorSet(SharedPtr<const FVulkanDescriptorSetLayout> _Layout) -> SharedPtr<FVulkanDescriptorSet>;

		struct FDescriptorEntry
		{
			EVulkanDescriptorType Type;
			UInt32			Count = 0;
		};
		auto inline
		AddEntry(EVulkanDescriptorType _Type, UInt32 _Count) -> FVulkanDescriptorPool* { DescriptorTable[_Type] += _Count; return this; }
		auto inline
		Build(UInt32 _MaxSets) -> SharedPtr<FVulkanDescriptorPool>;
		void inline
		Destroy();

		auto inline
		GetHandle() const -> const VkDescriptorPool {return Handle; }

		Bool inline
		HasBuilt() const { return Handle != VK_NULL_HANDLE; }

		FVulkanDescriptorPool() noexcept = default;
		~FVulkanDescriptorPool() = default;
	private:
		VkDescriptorPool						Handle{ VK_NULL_HANDLE };
		UInt32									MaxSets{ 0 };
		HashMap<EVulkanDescriptorType, UInt32>	DescriptorTable;
		List<SharedPtr<FVulkanDescriptorSet>>	Children;

	public:
		void CollectGarbages(Bool _bDestroyMode = False);
	};

	SharedPtr<FVulkanDescriptorPool> FVulkanDescriptorPool::
	Build(UInt32 _MaxSets)
	{
		MaxSets = _MaxSets; VE_ASSERT(MaxSets > 0);

		Array<VkDescriptorPoolSize> Entries(DescriptorTable.size());
		UInt32 EntryCount = 0;
		for (const auto& [ItemType, ItemCount] : DescriptorTable)
		{
			UInt32 Idx = EntryCount++;
			Entries[Idx].type            = AutoCast(ItemType);
			Entries[Idx].descriptorCount = ItemCount;
		}

		VkDescriptorPoolCreateInfo CreateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets		= _MaxSets,
			.poolSizeCount  = EntryCount,
			.pPoolSizes		= Entries.data()
		};

		if (vkCreateDescriptorPool(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create the Vulkan Descriptor Pool!"); }

		return shared_from_this();
	}

	void FVulkanDescriptorPool::
	Destroy()
	{
		CollectGarbages(True);
		vkDestroyDescriptorPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	void FVulkanDescriptorPool::
	CollectGarbages(Bool _bDestroyMode/* = False*/)
	{
		static Array<VkDescriptorSet> Trashbin(8);
		UInt32 CollectionCount = 0;
		
		if (_bDestroyMode)
		{
			for (auto& Child : Children)
			{
				if (Child->Handle == VK_NULL_HANDLE) { continue; }

				if (CollectionCount >= Trashbin.size())
				{ Trashbin.resize(Trashbin.size() << 1); }

				Trashbin[CollectionCount++] = Child->Handle;
				Child->Layout.reset();
				Child->Status				= FVulkanDescriptorSet::EStatus::Expired;
				Child->Handle				= VK_NULL_HANDLE;
			}
		}
		else
		{
			for (auto It = Children.begin(); It != Children.end(); )
			{
				auto& Child = *It;
				if (Child->Handle == VK_NULL_HANDLE) { continue; }

				if (Child->IsExpired() || Child.use_count() == 1)
				{
					if (CollectionCount >= Trashbin.size())
					{ Trashbin.resize(Trashbin.size() << 1); }

					Child->Layout.reset();
					Trashbin[CollectionCount++] = Child->Handle;
					Child->Handle = VK_NULL_HANDLE;
				
					Children.erase(It);
				}
				else { ++It; }
			}
		}

		if (CollectionCount)
		{ vkFreeDescriptorSets(GVulkan->Device->GetHandle(), Handle, CollectionCount, Trashbin.data()); }
	}

	SharedPtr<FVulkanDescriptorSet> FVulkanDescriptorPool::
	CreateDescriptorSet(SharedPtr<const FVulkanDescriptorSetLayout> _Layout)
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
	
} // namespace VE