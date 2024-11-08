module;
#include <Visera>

#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Synchronization;

import :Allocator;
import :Device;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanSemaphore
	{
		friend class Vulkan;
	public:
		auto GetHandle()	const	-> VkSemaphore { return Handle; }
		operator VkSemaphore() const { return Handle; }

		VulkanSemaphore() noexcept = delete;
		VulkanSemaphore(const VulkanDevice& Device, Bool bSignaled = False);
		~VulkanSemaphore();

	private:
		VkSemaphore				Handle{ VK_NULL_HANDLE };
		const VulkanDevice&		HostDevice;
	};

	class VulkanFence
	{
		friend class Vulkan;
	public:
		void Wait(UInt64 Timeout = UINT64_MAX) const { Assert(!IsSignaled()); vkWaitForFences(HostDevice.GetHandle(), 1, &Handle, VK_TRUE, Timeout); }
		void Reset()		const { Assert(IsSignaled()); vkResetFences(HostDevice.GetHandle(), 1, &Handle); }
		Bool IsSignaled()	const { return VK_SUCCESS == vkGetFenceStatus(HostDevice.GetHandle(), Handle); }

		auto GetHandle()	const -> VkFence { return Handle; }
		operator VkFence()	const { return Handle; }

		VulkanFence() noexcept = delete;
		VulkanFence(const VulkanDevice& Device, Bool bSignaled = False);
		~VulkanFence();

	private:
		VkFence					Handle{ VK_NULL_HANDLE };
		const VulkanDevice&		HostDevice;
	};

	VulkanSemaphore::
	VulkanSemaphore(const VulkanDevice& Device, Bool bSignaled/* = False*/)
		:HostDevice{Device}
	{
		VkSemaphoreCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkSemaphoreCreateFlags(0),
		};
		VK_CHECK(vkCreateSemaphore(
			HostDevice.GetHandle(),
			&CreateInfo,
			VulkanAllocator::AllocationCallbacks,
			&Handle));
	}

	VulkanSemaphore::
	~VulkanSemaphore()
	{
		vkDestroySemaphore(HostDevice.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	VulkanFence::
	VulkanFence(const VulkanDevice& Device, Bool bSignaled/* = False*/) 
		:HostDevice {Device}
	{
		VkFenceCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags(0),
		};
		VK_CHECK(vkCreateFence(
			HostDevice.GetHandle(),
			&CreateInfo,
			VulkanAllocator::AllocationCallbacks,
			&Handle));
	}

	VulkanFence::
	~VulkanFence()
	{
		vkDestroyFence(HostDevice.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime