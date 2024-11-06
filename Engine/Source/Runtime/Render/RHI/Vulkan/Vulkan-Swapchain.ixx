module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan:Swapchain;

import :Allocator;
import :Device;

export namespace VE { namespace Render
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanSwapchain
	{
		friend class VulkanContext;
	public:
		auto GetHandle()			const	-> VkSwapchainKHR { return Handle; }
		operator VkSwapchainKHR()	const { return Handle; }

	private:
		VkSwapchainKHR      Handle{ VK_NULL_HANDLE };

		void Create(const VulkanDevice& Device);
		void Destroy(const VulkanDevice& Device);

	public:
		VulkanSwapchain() noexcept = default;
		~VulkanSwapchain() noexcept = default;
	};

	void VulkanSwapchain::
	Create(const VulkanDevice& Device)
	{
		
	}

	void VulkanSwapchain::
	Destroy(const VulkanDevice& Device)
	{
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Render