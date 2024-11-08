module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Swapchain;

import :Allocator;
import :Device;
import :Surface;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanSwapchain
	{
		friend class Vulkan;
	public:
		auto GetHandle()			const	-> VkSwapchainKHR { return Handle; }
		operator VkSwapchainKHR()	const { return Handle; }

	private:
		VkSwapchainKHR			Handle{ VK_NULL_HANDLE };
		const VulkanDevice&		HostDevice;
		const VulkanSurface&	HostSurface;

		void Create();
		void Destroy();

	public:
		VulkanSwapchain(const VulkanDevice& Device, const VulkanSurface& Suface) noexcept : HostDevice{ Device }, HostSurface{ Suface } {};
		~VulkanSwapchain() noexcept = default;
	};

	void VulkanSwapchain::
	Create()
	{
		
	}

	void VulkanSwapchain::
	Destroy()
	{
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime