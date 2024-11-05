module;
#include <ViseraEngine>

#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Runtime.Render.RHI.Vulkan:Surface;

import Visera.Runtime.Platform;
import :Allocator;
import :Instance;

export namespace VE { namespace RHI
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanSurface
	{
		friend class VulkanContext;
	public:
		auto GetHandle()		const	-> VkSurfaceKHR						{ return Handle; }
		void SetFormats(Array<VkSurfaceFormatKHR>&&		NewFormats)		{ Formats		= std::move(NewFormats); }
		void SetPresentModes(Array<VkPresentModeKHR>&&	NewPresentModes){ PresentModes	= std::move(NewPresentModes); }
		operator VkSurfaceKHR() const { return Handle; }

	private:
		VkSurfaceKHR                Handle{ VK_NULL_HANDLE };
		Array<VkSurfaceFormatKHR>	Formats;
		Array<VkPresentModeKHR>		PresentModes;


		void Create(const VulkanInstance& Instance);
		void Destroy(const VulkanInstance& Instance);

	public:
		VulkanSurface() noexcept = default;
		~VulkanSurface() noexcept = default;
	};

	void VulkanSurface::
	Create(const VulkanInstance& Instance)
	{
		VK_CHECK(glfwCreateWindowSurface(
			Instance,
			PlatformRuntime::GetWindow().GetHandle(),
			VulkanAllocator::AllocationCallbacks,
			&Handle));
	}

	void VulkanSurface::
	Destroy(const VulkanInstance& Instance)
	{
		vkDestroySurfaceKHR(Instance, Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
} } // namespace VE::RHI