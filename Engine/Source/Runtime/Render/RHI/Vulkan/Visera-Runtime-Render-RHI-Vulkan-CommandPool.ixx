module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:CommandPool;

import :Enums;
import :Device;
import :Synchronization;

import Visera.Core.Signal;

VISERA_PUBLIC_MODULE
class VulkanCommandPool
{
	friend class RHI;
	friend class Vulkan;
public:
	class CommandBuffer
	{
		friend class VulkanCommandPool;
	public:
		enum Level
		{
			Primary		= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			Secondary	= VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		};
	public:
		void StartRecording();
		void StopRecording();
		void Reset() { VE_ASSERT(!IsRecording() && HostCommandPool.IsResetable()); vkResetCommandBuffer(Handle, 0x0); }

		Bool IsRecording()	const { return bRecording; }
		Bool IsPrimary()	const { return Type == VK_COMMAND_BUFFER_LEVEL_PRIMARY; }
		auto GetLevel()		const -> VkCommandBufferLevel { return Type; }

		auto GetHandle() const -> VkCommandBuffer { return Handle; }
		operator VkCommandBuffer() const { return Handle; }

	private:
		VkCommandBuffer				Handle{ VK_NULL_HANDLE };
		const VulkanCommandPool&	HostCommandPool;
		VkCommandBufferLevel		Type;
		Bool						bRecording{ False };

	public:
		CommandBuffer(const VulkanCommandPool& CommandPool, Level Type) noexcept;
		CommandBuffer() noexcept = delete;
		~CommandBuffer() noexcept;
	};

	auto GetType() const -> ECommandPool { return ECommandPool(Type); }
	Bool IsResetable() const { return Type == ECommandPool::Resetable; }
	auto Allocate(CommandBuffer::Level Level) const -> SharedPtr<CommandBuffer>;
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
	VulkanCommandPool()  noexcept = default;
	~VulkanCommandPool() noexcept = default;

private:
	VkCommandPool				Handle{ VK_NULL_HANDLE };
	ECommandPool				Type;
	EQueueFamily				QueueFamilyType;
	Array<VkCommandBuffer>		RecycleBin;
};

void VulkanCommandPool::
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
	{ throw RuntimeError("Failed to create Vulkan CommandPool!"); }
}

void VulkanCommandPool::
Destroy()
{
	EmptyRecycleBin();
	vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

void VulkanCommandPool::
EmptyRecycleBin()
{
	if (RecycleBin.empty()) return;
	vkFreeCommandBuffers(GVulkan->Device->GetHandle(), Handle, RecycleBin.size(), RecycleBin.data());
}

SharedPtr<VulkanCommandPool::CommandBuffer> VulkanCommandPool::
Allocate(CommandBuffer::Level Level) const
{
	auto CommandBuffer = CreateSharedPtr<VulkanCommandPool::CommandBuffer>(*this, Level);
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
	{ throw RuntimeError("Failed to create Vulkan CommandBuffer!"); }

	return CommandBuffer;
}

void VulkanCommandPool::
Free(VkCommandBuffer CommandBuffer)  const
{
	auto& Bin = const_cast<decltype(RecycleBin)&>(RecycleBin);
	Bin.emplace_back(CommandBuffer);
}

void VulkanCommandPool::
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

VulkanCommandPool::CommandBuffer::
CommandBuffer(const VulkanCommandPool& CommandPool, Level Type) noexcept
	:HostCommandPool{ CommandPool }, Type{ VkCommandBufferLevel(Type) }
{

}

VulkanCommandPool::CommandBuffer::
~CommandBuffer() noexcept
{
	HostCommandPool.Free(Handle);
	Handle = VK_NULL_HANDLE;
}

void VulkanCommandPool::CommandBuffer::
StartRecording()
{
	VE_ASSERT(!IsRecording());

	VkCommandBufferBeginInfo BeginInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0x0,
		.pInheritanceInfo = nullptr
	};
	if (VK_SUCCESS != vkBeginCommandBuffer(Handle, &BeginInfo))
	{ throw RuntimeError("Failed to begin recording Vulkan Command Buffer!"); }

	bRecording = True;
}

void VulkanCommandPool::CommandBuffer::
StopRecording()
{
	VE_ASSERT(IsRecording());

	if (VK_SUCCESS != vkEndCommandBuffer(Handle))
	{ throw RuntimeError("Failed to stop recording Vulkan Command Buffer!"); }

	bRecording = False;
}
VISERA_MODULE_END