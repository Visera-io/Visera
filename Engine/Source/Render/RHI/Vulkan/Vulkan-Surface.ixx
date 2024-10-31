module;
#include <ViseraEngine>

#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Render.RHI.Vulkan:Surface;

import Visera.Platform;
import :Allocator;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanSurface
	{
		friend class VulkanContext;
	private:
		VkSurfaceKHR                Handle{ VK_NULL_HANDLE };
		Array<VkSurfaceFormatKHR>	Formats;
		Array<VkPresentModeKHR>		PresentModes;
		operator VkSurfaceKHR() const { return Handle; }

		void Create(VkInstance Instance);
		void Destroy(VkInstance Instance);

	public:
		VulkanSurface() noexcept = default;
		~VulkanSurface() noexcept = default;
	};

	void VulkanSurface::
	Create(VkInstance Instance)
	{
		VK_CHECK(glfwCreateWindowSurface(
			Instance,
			ViseraPlatform::GetWindow().GetHandle(),
			VulkanAllocator::AllocationCallbacks,
			&Handle));
	}

	void VulkanSurface::
	Destroy(VkInstance Instance)
	{
		vkDestroySurfaceKHR(Instance, Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
} // namespace VE