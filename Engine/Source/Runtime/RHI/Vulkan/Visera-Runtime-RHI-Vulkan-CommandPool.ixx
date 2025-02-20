module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:CommandPool;

import :Common;
import :Device;
import :Synchronization;
import :CommandBuffer;

import Visera.Core.Signal;

export namespace VE
{
	class RHI;

	class FVulkanCommandPool : public std::enable_shared_from_this<FVulkanCommandPool>
	{
		friend class RHI;
	public:
		auto GetHandle()	const -> const VkCommandPool { return Handle; }

		Bool IsResetable()	const { return PoolType == EVulkanCommandPoolType::Resetable; }

	protected:
		VkCommandPool						Handle{ VK_NULL_HANDLE };
		EVulkanCommandPoolType				PoolType;
		EVulkanQueueFamily					QueueFamilyType;
		List<SharedPtr<FVulkanCommandBuffer>>Children;
		Array<VkCommandBuffer>				Trashbin;

	public:
		FVulkanCommandPool() = delete;
		FVulkanCommandPool(EVulkanCommandPoolType _PoolType, EVulkanQueueFamily _QueueFamily);
		~FVulkanCommandPool();

	private:
		void CollectGarbages(Bool _bDestroyMode = False); // Managed by RHI or Deconstructor
	};

	class FVulkanGraphicsCommandPool : public FVulkanCommandPool
	{
	public:
		auto CreateGraphicsCommandBuffer(EVulkanCommandLevel _CommandLevel) -> SharedPtr<FVulkanGraphicsCommandBuffer>;

	public:
		FVulkanGraphicsCommandPool() = delete;
		FVulkanGraphicsCommandPool(EVulkanCommandPoolType _PoolType)
		:FVulkanCommandPool{_PoolType, EVulkanQueueFamily::Graphics}{ }
	};
	
	FVulkanCommandPool::
	FVulkanCommandPool(EVulkanCommandPoolType _PoolType, EVulkanQueueFamily _QueueFamily)
		:PoolType {_PoolType},
		 QueueFamilyType {_QueueFamily}
	{
		VkCommandPoolCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = AutoCast(PoolType),
			.queueFamilyIndex = GVulkan->Device->GetQueueFamily(QueueFamilyType).Index,
		};

		if(VK_SUCCESS != vkCreateCommandPool(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan FCommandPool!"); }

		Trashbin.resize(8);
	}

	FVulkanCommandPool::
	~FVulkanCommandPool()
	{
		CollectGarbages(True);
		vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	SharedPtr<FVulkanGraphicsCommandBuffer> FVulkanGraphicsCommandPool::
	CreateGraphicsCommandBuffer(EVulkanCommandLevel _CommandLevel)
	{
		auto CommandBuffer = CreateSharedPtr<FVulkanGraphicsCommandBuffer>(shared_from_this(), _CommandLevel);
		CommandBuffer->bIsResettable = IsResetable();

		VkCommandBufferAllocateInfo AllocateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool	= Handle,
			.level			= AutoCast(CommandBuffer->GetLevel()),
			.commandBufferCount = 1
		};

		if(vkAllocateCommandBuffers(
			GVulkan->Device->GetHandle(),
			&AllocateInfo,
			&CommandBuffer->Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create FGraphicsCommandBuffer!"); }

		Children.push_back(CommandBuffer);
		return CommandBuffer;
	}

	void FVulkanCommandPool::
	CollectGarbages(Bool _bDestroyMode/* = False*/)
	{
		UInt32 CollectionCount = 0;
		
		if (_bDestroyMode)
		{
			for (auto& Child : Children)
			{
				if (CollectionCount >= Trashbin.size())
				{ Trashbin.resize(Trashbin.size() << 1); }

				Trashbin[CollectionCount++] = Child->Handle;
				Child->Status = FVulkanCommandBuffer::EStatus::Expired;
				Child->Handle = VK_NULL_HANDLE;
			}
		}
		else
		{
			for (auto It = Children.begin(); It != Children.end(); )
			{
				auto& Child = *It;
				if (Child->IsExpired() || Child.use_count() == 1)
				{
					if (CollectionCount >= Trashbin.size())
					{ Trashbin.resize(Trashbin.size() << 1); }

					Trashbin[CollectionCount++] = Child->Handle;
					Child->Handle = VK_NULL_HANDLE;
				
					Children.erase(It);
				}
				else { ++It; }
			}
		}

		if (CollectionCount)
		{ vkFreeCommandBuffers(GVulkan->Device->GetHandle(), Handle, CollectionCount, Trashbin.data()); }
	}


} // namespace VE