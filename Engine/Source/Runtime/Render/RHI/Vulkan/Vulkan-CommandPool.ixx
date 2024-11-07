module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:CommandPool;

import :Allocator;
import :Device;

import Visera.Engine.Core.Log;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanCommandPool
	{
		friend class VulkanContext;
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
			Bool IsRecording() const { return bRecording; }
			void BeginRecording();
			void StopRecording();

		private:
			VkCommandBuffer				Handle{ VK_NULL_HANDLE };
			const VulkanCommandPool&	HostCommandPool;
			Bool						bRecording{ False };

		public:
			CommandBuffer(const VulkanCommandPool& CommandPool) noexcept;
			CommandBuffer() noexcept = delete;
			~CommandBuffer() noexcept;
		};

		auto Allocate(CommandBuffer::Level Level) const -> SharedPtr<CommandBuffer>;
		void Free(VkCommandBuffer CommandBuffer)  const;

		auto GetHandle() const { return Handle; }
		operator VkCommandPool() const { return Handle; }

	private:
		void Create(VulkanDevice::QueueFamilyType QueueFamilyType, PoolType Type);
		void Destroy();
		void EmptyRecycleBin();

	public:
		VulkanCommandPool(const VulkanDevice& Device) noexcept :HostDevice{ Device } {}
		VulkanCommandPool() noexcept = delete;
		~VulkanCommandPool() noexcept = default;

	private:
		VkCommandPool						Handle{ VK_NULL_HANDLE };
		const VulkanDevice&					HostDevice;
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
			.queueFamilyIndex = HostDevice.GetQueueFamily(QueueFamilyType).Index,
		};

		VK_CHECK(vkCreateCommandPool(HostDevice.GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));
	}

	void VulkanCommandPool::
	Destroy()
	{
		EmptyRecycleBin();
		vkDestroyCommandPool(HostDevice.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	void VulkanCommandPool::
	EmptyRecycleBin()
	{
		if (RecycleBin.empty()) return;
		vkFreeCommandBuffers(HostDevice.GetHandle(), Handle, RecycleBin.size(), RecycleBin.data());
	}

	SharedPtr<VulkanCommandPool::CommandBuffer> VulkanCommandPool::
	Allocate(CommandBuffer::Level Level) const
	{
		auto CommandBuffer = CreateSharedPtr<VulkanCommandPool::CommandBuffer>(*this);
		VkCommandBufferAllocateInfo AllocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = Handle,
			.level = VkCommandBufferLevel(Level),
			.commandBufferCount = 1
		};
		VK_CHECK(vkAllocateCommandBuffers(HostDevice.GetHandle(), &AllocateInfo, &CommandBuffer->Handle));

		return CommandBuffer;
	}

	void VulkanCommandPool::
	Free(VkCommandBuffer CommandBuffer)  const
	{
		auto& Bin = const_cast<decltype(RecycleBin)&>(RecycleBin);
		Bin.emplace_back(CommandBuffer);
	}

	VulkanCommandPool::CommandBuffer::
	CommandBuffer(const VulkanCommandPool& CommandPool) noexcept
		:HostCommandPool{ CommandPool }
	{

	}

	VulkanCommandPool::CommandBuffer::
	~CommandBuffer() noexcept
	{
		HostCommandPool.Free(Handle);
		Handle = VK_NULL_HANDLE;
	}

	void VulkanCommandPool::CommandBuffer::
	BeginRecording()
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