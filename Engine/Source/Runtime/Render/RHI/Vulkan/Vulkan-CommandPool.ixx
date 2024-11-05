module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan:CommandPool;

import :Allocator;
import :Device;

import Visera.Core.Log;

export namespace VE { namespace RHI
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanCommandPool
	{
		friend class VulkanContext;
	public:
		enum CommandBufferType
		{
			Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			Resetable = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
		};
		class CommandBuffer
		{
		public:
			Bool IsRecording() const { return bRecording; }
			void BeginRecording();
			void StopRecording();

		private:
			VkCommandBuffer Handle{ VK_NULL_HANDLE };
			Bool bRecording{ False };
		};
	private:
		void Create(VulkanDevice::QueueFamilyType QueueFamilyType, CommandBufferType CommandBufferType);
		void Destroy();

	public:
		VulkanCommandPool(const VulkanDevice& Device) noexcept :HostDevice{ Device } {}
		VulkanCommandPool() noexcept = delete;
		~VulkanCommandPool() noexcept = default;

	private:
		VkCommandPool						Handle{ VK_NULL_HANDLE };
		const VulkanDevice&					HostDevice;
		VkCommandPoolCreateFlags			Type;
		VulkanDevice::QueueFamilyType		QueueFamilyType;	
	};

	void VulkanCommandPool::
	Create(VulkanDevice::QueueFamilyType QueueFamilyType, CommandBufferType CommandBufferType)
	{
		Type = CommandBufferType;
		QueueFamilyType = QueueFamilyType;

		VkCommandPoolCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = Type,
			.queueFamilyIndex = HostDevice.GetQueueFamily(QueueFamilyType).Index,
		};

		VK_CHECK(vkCreateCommandPool(HostDevice.GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));
	}

	void VulkanCommandPool::
	Destroy()
	{
		vkDestroyCommandPool(HostDevice.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
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

} } // namespace VE::RHI