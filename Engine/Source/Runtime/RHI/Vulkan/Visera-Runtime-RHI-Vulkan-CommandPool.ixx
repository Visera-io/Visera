module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:CommandPool;

import :Enums;
import :Device;
import :Synchronization;
import :CommandBuffer;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkan;
	class RHI;

	class FVulkanCommandPool : public std::enable_shared_from_this<FVulkanCommandPool>
	{
		friend class FVulkan;
		friend class RHI;
	public:
		auto CreateCommandBuffer(ECommandLevel _CommandLevel) -> SharedPtr<FVulkanCommandBuffer>;

		auto GetType() const -> ECommandPool { return ECommandPool(Type); }
		Bool IsResetable() const { return Type == ECommandPool::Resetable; }

		auto GetHandle() { return Handle; }

	private:
		void Create(EQueueFamily QueueFamilyType, ECommandPool Type);
		void Destroy();
		void CollectGarbages(Bool _bDestroyMode = False); // Managed by RHI or Deconstructor

	public:
		FVulkanCommandPool()  noexcept = default;
		~FVulkanCommandPool() noexcept = default;

	private:
		VkCommandPool				Handle{ VK_NULL_HANDLE };
		ECommandPool				Type;
		EQueueFamily				QueueFamilyType;
		List<SharedPtr<FVulkanCommandBuffer>> Children;
	};

	void FVulkanCommandPool::
	Create(EQueueFamily _QueueFamilyType, ECommandPool _CommandPoolType)
	{
		Type = _CommandPoolType;
		QueueFamilyType = _QueueFamilyType;

		VkCommandPoolCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = AutoCast(Type),
			.queueFamilyIndex = GVulkan->Device->GetQueueFamily(_QueueFamilyType).Index,
		};

		if(VK_SUCCESS != vkCreateCommandPool(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan FCommandPool!"); }
	}

	void FVulkanCommandPool::
	Destroy()
	{
		CollectGarbages(True);
		vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	SharedPtr<FVulkanCommandBuffer> FVulkanCommandPool::
	CreateCommandBuffer(ECommandLevel _CommandLevel)
	{
		auto CommandBuffer = CreateSharedPtr<FVulkanCommandBuffer>(shared_from_this(), _CommandLevel);
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
		{ throw SRuntimeError("Failed to create FCommandBuffer!"); }

		return Children.emplace_back(std::move(CommandBuffer));
	}

	void FVulkanCommandPool::
	CollectGarbages(Bool _bDestroyMode/* = False*/)
	{
		static Array<VkCommandBuffer> Trashbin(8);
		UInt32 CollectionCount = 0;
		
		if (_bDestroyMode)
		{
			for (auto& Child : Children)
			{
				if (CollectionCount >= Trashbin.size())
				{ Trashbin.resize(Trashbin.size() << 1); }

				Trashbin[CollectionCount++] = Child->Handle;
				Child->Status = FVulkanCommandBuffer::EStatus::Zombie;
				Child->Handle = VK_NULL_HANDLE;
			}
		}
		else
		{
			for (auto It = Children.begin(); It != Children.end(); )
			{
				auto& Child = *It;
				if (Child->IsZombie() || Child.use_count() == 1)
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


} } // namespace VE::Runtime