module;
#include <Visera>

#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Synchronization;

import :Context;
import :Device;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanSemaphore
	{
		friend class Vulkan;
	public:
		auto GetHandle()		const	-> VkSemaphore { return Handle; }
		operator VkSemaphore()	const { return Handle; }

		VulkanSemaphore(Bool bSignaled = False);
		~VulkanSemaphore();

	private:
		VkSemaphore				Handle{ VK_NULL_HANDLE };
	};

	class VulkanFence
	{
		friend class Vulkan;
	public:
		enum WaitTime : UInt64 { Forever = UINT64_MAX };
		void Wait(WaitTime Timeout = Forever) const { vkWaitForFences(GVulkan->Device->GetHandle(), 1, &Handle, VK_TRUE, Timeout); }
		void Lock()		const { Assert(!IsLocked()); vkResetFences(GVulkan->Device->GetHandle(), 1, &Handle); }
		Bool IsLocked()	const { return VK_SUCCESS != vkGetFenceStatus(GVulkan->Device->GetHandle(), Handle); }

		auto GetHandle()	const -> VkFence { return Handle; }
		operator VkFence()	const { return Handle; }

		VulkanFence(Bool bSignaled = False);
		~VulkanFence();

	private:
		VkFence					Handle{ VK_NULL_HANDLE };
	};

	VulkanSemaphore::
	VulkanSemaphore(Bool bSignaled/* = False*/)
	{
		VkSemaphoreCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkSemaphoreCreateFlags(0),
		};
		VK_CHECK(vkCreateSemaphore(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle));
	}

	VulkanSemaphore::
	~VulkanSemaphore()
	{
		vkDestroySemaphore(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	VulkanFence::
	VulkanFence(Bool bSignaled/* = False*/) 
	{
		VkFenceCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags(0),
		};
		VK_CHECK(vkCreateFence(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle));
	}

	VulkanFence::
	~VulkanFence()
	{
		vkDestroyFence(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime