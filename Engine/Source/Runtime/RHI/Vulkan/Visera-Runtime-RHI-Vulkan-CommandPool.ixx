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

	class FVulkanCommandPool
	{
		friend class FVulkan;
		friend class RHI;
	public:
		auto GetType() const -> ECommandPool { return ECommandPool(Type); }
		Bool IsResetable() const { return Type == ECommandPool::Resetable; }
		auto CreateCommandBuffer(ECommandLevel _CommandLevel) const -> SharedPtr<FVulkanCommandBuffer> { auto Result = CreateSharedPtr<FVulkanCommandBuffer>(Handle, _CommandLevel); Result->bIsResettable = IsResetable(); return Result; }

		struct SubmitInfo
		{
			Array<VkPipelineStageFlags> Deadlines;
			Array<VkCommandBuffer>		CommandBuffers;
			Array<VkSemaphore>			WaitSemaphores;
			Array<VkSemaphore>			SignalSemaphores;
			VkFence						Fence = VK_NULL_HANDLE;
		};
		void Submit(const SubmitInfo& SubmitInfo) const;

		auto GetHandle()		  { return Handle; }
		operator VkCommandPool()  { return Handle; }

	private:
		void Create(EQueueFamily QueueFamilyType, ECommandPool Type);
		void Destroy();

	public:
		FVulkanCommandPool()  noexcept = default;
		~FVulkanCommandPool() noexcept = default;

	private:
		VkCommandPool				Handle{ VK_NULL_HANDLE };
		ECommandPool				Type;
		EQueueFamily				QueueFamilyType;
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
		vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
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
		vkQueueSubmit(Queue, 1, &FinalSubmitInfo, SubmitInfo.Fence);
	}

} } // namespace VE::Runtime