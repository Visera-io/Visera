module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:CommandPool;

import :Enums;
import :Device;
import :Synchronization;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanCommandPool
	{
		friend class RHI;
		friend class FVulkan;
	public:
		auto GetType() const -> ECommandPool { return ECommandPool(Type); }
		Bool IsResetable() const { return Type == ECommandPool::Resetable; }
		auto Allocate(ECommandLevel _CommandLevel) const -> SharedPtr<FVulkanCommandBuffer>;
		void Free(VkCommandBuffer CommandBuffer)  const;
		struct SubmitInfo
		{
			Array<VkPipelineStageFlags> Deadlines;
			Array<VkCommandBuffer>		CommandBuffers;
			Array<VkSemaphore>			WaitSemaphores;
			Array<VkSemaphore>			SignalSemaphores;
			VkFence						Fence = VK_NULL_HANDLE;
		};
		void Submit(const SubmitInfo& SubmitInfo) const;

		auto GetHandle() const { return Handle; }
		operator VkCommandPool() const { return Handle; }

	private:
		void Create(EQueueFamily QueueFamilyType, ECommandPool Type);
		void Destroy();
		void EmptyRecycleBin();

	public:
		FVulkanCommandPool()  noexcept = default;
		~FVulkanCommandPool() noexcept = default;

	private:
		VkCommandPool				Handle{ VK_NULL_HANDLE };
		ECommandPool				Type;
		EQueueFamily				QueueFamilyType;
		mutable Array<VkCommandBuffer> RecycleBin;
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
			.queueFamilyIndex = GVulkan->Device->GetQueueFamily(QueueFamilyType).Index,
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
		EmptyRecycleBin();
		vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	void FVulkanCommandPool::
	EmptyRecycleBin()
	{
		if (!RecycleBin.empty())
		{
			vkFreeCommandBuffers(GVulkan->Device->GetHandle(), Handle, RecycleBin.size(), RecycleBin.data());
		}
	}

	SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> FVulkanCommandPool::
	Allocate(ECommandLevel _CommandLevel) const
	{
		auto CommandBuffer = CreateSharedPtr<FVulkanCommandPool::FVulkanCommandBuffer>(*this, _CommandLevel);
		VkCommandBufferAllocateInfo AllocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = Handle,
			.level = CommandBuffer->GetLevel(),
			.commandBufferCount = 1
		};
		if(VK_SUCCESS != vkAllocateCommandBuffers(
			GVulkan->Device->GetHandle(),
			&AllocateInfo,
			&CommandBuffer->Handle))
		{ throw SRuntimeError("Failed to create Vulkan FCommandBuffer!"); }

		return CommandBuffer;
	}

	void FVulkanCommandPool::
	Free(VkCommandBuffer CommandBuffer)  const
	{
		RecycleBin.emplace_back(CommandBuffer);
	}

	void FVulkanCommandPool::
	Submit(const SubmitInfo& SubmitInfo) const
	{
		VkSubmitInfo FinalSubmitInfo
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = UInt32(SubmitInfo.WaitSemaphores.size()),
			.pWaitSemaphores	= SubmitInfo.WaitSemaphores.data(),
			.pWaitDstStageMask  = SubmitInfo.Deadlines.data(),
			.commandBufferCount = UInt32(SubmitInfo.CommandBuffers.size()),
			.pCommandBuffers	= SubmitInfo.CommandBuffers.data(),
			.signalSemaphoreCount = UInt32(SubmitInfo.SignalSemaphores.size()),
			.pSignalSemaphores	= SubmitInfo.SignalSemaphores.data()
		};
		
		VkQueue Queue = GVulkan->Device->GetQueueFamily(QueueFamilyType).Queues.front();
		//[FIXME]: Revise the last parameter.
		vkQueueSubmit(Queue, 1, &FinalSubmitInfo, SubmitInfo.Fence);
	}

} } // namespace VE::Runtime