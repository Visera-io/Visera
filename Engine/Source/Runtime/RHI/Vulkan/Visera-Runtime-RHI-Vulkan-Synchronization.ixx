module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Synchronization;

import Visera.Core.Signal;

import :Common;
import :Device;

export namespace VE { namespace Runtime
{
	class RHI;
	class FVulkanImage;

	class FVulkanSemaphore
	{
		VE_NOT_COPYABLE(FVulkanSemaphore);
		friend class RHI;
	public:
		auto GetHandle() const -> const VkSemaphore { return Handle; }
		operator const VkSemaphore() const { return Handle; }

		FVulkanSemaphore(Bool bSignaled = False);
		~FVulkanSemaphore();
		FVulkanSemaphore(FVulkanSemaphore&& _Another) : Handle{ _Another.Handle } { _Another.Handle = VK_NULL_HANDLE; }
		FVulkanSemaphore& operator=(FVulkanSemaphore&& _Another) { Handle = _Another.Handle; _Another.Handle = VK_NULL_HANDLE; return *this; }

	private:
		VkSemaphore	Handle{ VK_NULL_HANDLE };
	};
	static_assert(sizeof(FVulkanSemaphore) == sizeof(VkFence));
	static_assert(std::is_standard_layout_v<FVulkanSemaphore>);
	
	class FVulkanFence
	{
		VE_NOT_COPYABLE(FVulkanFence);
		friend class RHI;
	public:
		enum class EStatus { Blocking = 0, Signaled = VK_FENCE_CREATE_SIGNALED_BIT };
		auto GetHandle() const -> const VkFence { return Handle; }
		operator const VkFence() const { return Handle; }

		enum WaitTime : UInt64 { Forever = UINT64_MAX };
		void Wait(WaitTime Timeout = Forever) const { vkWaitForFences(GVulkan->Device->GetHandle(), 1, &Handle, VK_TRUE, Timeout); }
		void Block()		const { VE_ASSERT(!IsBlocking()); vkResetFences(GVulkan->Device->GetHandle(), 1, &Handle); }
		Bool IsBlocking()	const { return VK_SUCCESS != vkGetFenceStatus(GVulkan->Device->GetHandle(), Handle); }

		FVulkanFence(EStatus _Status = EStatus::Blocking);
		~FVulkanFence();
		FVulkanFence(FVulkanFence&& _Another) : Handle{ _Another.Handle } { _Another.Handle = VK_NULL_HANDLE; }
		FVulkanFence& operator=(FVulkanFence&& _Another) { Handle = _Another.Handle; _Another.Handle = VK_NULL_HANDLE; return *this; }

	private:
		VkFence	 Handle{ VK_NULL_HANDLE };
	};
	static_assert(sizeof(FVulkanFence) == sizeof(VkFence));
	static_assert(std::is_standard_layout_v<FVulkanFence>);

	struct FVulkanImageMemoryBarrier
	{
		SharedPtr<const FVulkanImage> Image;
		EVulkanImageLayout			  NewLayout;
		EVulkanAccess				  SrcAccess;
		EVulkanAccess				  DstAccess;

		UInt32 SrcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		UInt32 DstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
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
		{ throw SRuntimeError("Failed to create Vulkan FSemaphore!"); }
	}

	FVulkanSemaphore::
	~FVulkanSemaphore()
	{
		if (Handle != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

	FVulkanFence::
	FVulkanFence(EStatus _Status/* = EStatus::Blocking*/)
	{
		VkFenceCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = UInt32(_Status),
		};
		if(VK_SUCCESS != vkCreateFence(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan FFence!"); }
	}

	FVulkanFence::
	~FVulkanFence()
	{
		if (Handle != VK_NULL_HANDLE)
		{
			vkDestroyFence(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

} } // namespace VE::Runtime