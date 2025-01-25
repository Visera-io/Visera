module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Synchronization;

import Visera.Core.Signal;

import :Device;

export namespace VE { namespace Runtime
{


class FVulkanSemaphore
{
	friend class FVulkan;
public:
	auto GetHandle()		const	-> VkSemaphore { return Handle; }
	operator VkSemaphore()	const { return Handle; }

	FVulkanSemaphore(Bool bSignaled = False);
	~FVulkanSemaphore();

private:
	VkSemaphore				Handle{ VK_NULL_HANDLE };
};

class FVulkanFence
{
	friend class FVulkan;
public:
	enum WaitTime : UInt64 { Forever = UINT64_MAX };
	void Wait(WaitTime Timeout = Forever) const { vkWaitForFences(GVulkan->Device->GetHandle(), 1, &Handle, VK_TRUE, Timeout); }
	void Lock()		const { VE_ASSERT(!IsLocked()); vkResetFences(GVulkan->Device->GetHandle(), 1, &Handle); }
	Bool IsLocked()	const { return VK_SUCCESS != vkGetFenceStatus(GVulkan->Device->GetHandle(), Handle); }

	auto GetHandle()	const -> VkFence { return Handle; }
	operator VkFence()	const { return Handle; }

	FVulkanFence(Bool bSignaled = False);
	~FVulkanFence();

private:
	VkFence					Handle{ VK_NULL_HANDLE };
};

FVulkanSemaphore::
FVulkanSemaphore(Bool bSignaled/* = False*/)
{
	VkSemaphoreCreateInfo CreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkSemaphoreCreateFlags(0),
	};
	if(VK_SUCCESS != vkCreateSemaphore(
		GVulkan->Device->GetHandle(),
		&CreateInfo,
		GVulkan->AllocationCallbacks,
		&Handle))
	{ throw SRuntimeError("Failed to create Vulkan Semaphore!"); }
}

FVulkanSemaphore::
~FVulkanSemaphore()
{
	vkDestroySemaphore(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

FVulkanFence::
FVulkanFence(Bool bSignaled/* = False*/) 
{
	VkFenceCreateInfo CreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = bSignaled? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags(0),
	};
	if(VK_SUCCESS != vkCreateFence(
		GVulkan->Device->GetHandle(),
		&CreateInfo,
		GVulkan->AllocationCallbacks,
		&Handle))
	{ throw SRuntimeError("Failed to create Vulkan Fence!"); }
}

FVulkanFence::
~FVulkanFence()
{
	vkDestroyFence(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

} } // namespace VE::Runtime