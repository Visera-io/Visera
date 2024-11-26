module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:CommandPool;

import :Context;
import :Allocator;
import :Device;
import :Synchronization;

import Visera.Engine.Core.Log;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class RHI;
	class Vulkan;

	class VulkanCommandPool
	{
		friend class RHI;
		friend class Vulkan;
	public:
		enum PoolType
		{
			Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			Resetable = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
		};

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
			void Reset() { Assert(!IsRecording() && HostCommandPool.IsResetable()); vkResetCommandBuffer(Handle, 0x0); }

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

		auto GetType() const -> PoolType { return PoolType(Type); }
		Bool IsResetable() const { return Type == PoolType::Resetable; }
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
		void Create(VulkanDevice::QueueFamilyType QueueFamilyType, PoolType Type);
		void Destroy();
		void EmptyRecycleBin();

	public:
		VulkanCommandPool()  noexcept = default;
		~VulkanCommandPool() noexcept = default;

	private:
		VkCommandPool						Handle{ VK_NULL_HANDLE };
		VkCommandPoolCreateFlags			Type;
		VulkanDevice::QueueFamilyType		QueueFamilyType;
		Array<VkCommandBuffer>				RecycleBin;
	};

	void VulkanCommandPool::
	Create(VulkanDevice::QueueFamilyType QueueFamilyType, PoolType Type)
	{
		this->Type = Type;
		QueueFamilyType = QueueFamilyType;

		VkCommandPoolCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = this->Type,
			.queueFamilyIndex = GVulkan->Device->GetQueueFamily(QueueFamilyType).Index,
		};

		VK_CHECK(vkCreateCommandPool(GVulkan->Device->GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));
	}

	void VulkanCommandPool::
	Destroy()
	{
		EmptyRecycleBin();
		vkDestroyCommandPool(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
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
		VK_CHECK(vkAllocateCommandBuffers(GVulkan->Device->GetHandle(), &AllocateInfo, &CommandBuffer->Handle));

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
		//{ Log::Fatal("Failed to submit current commandbuffers!"); }
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
		Assert(!IsRecording());

		VkCommandBufferBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (VK_SUCCESS != vkBeginCommandBuffer(Handle, &BeginInfo))
		{ Log::Fatal("Failed to begin recording Vulkan Command Buffer!"); }

		bRecording = True;
	}

	void VulkanCommandPool::CommandBuffer::
	StopRecording()
	{
		Assert(IsRecording());

		if (VK_SUCCESS != vkEndCommandBuffer(Handle))
		{ Log::Fatal("Failed to stop recording Vulkan Command Buffer!"); }

		bRecording = False;
	}

} } // namespace VE::Runtime