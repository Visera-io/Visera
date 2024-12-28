module;
#include "../VulkanPC.h"
export module Visera.Runtime.Render.RHI.Vulkan:Surface;

import Visera.Runtime.Platform;
import :Instance;
import :GPU;

export namespace VE
{
	class VulkanSurface
	{
		friend class Vulkan;
	public:
		auto GetCapabilities()	const	-> const VkSurfaceCapabilitiesKHR&	{ auto& C = const_cast<VkSurfaceCapabilitiesKHR&>(Capacities); vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GVulkan->GPU->GetHandle(), Handle, &C); return Capacities; }
		auto GetFormats()		const	-> const Array<VkSurfaceFormatKHR>& { return Formats; }
		auto GetPresentModes()	const	-> const Array<VkPresentModeKHR>&	{ return PresentModes; }
		void SetFormats(Array<VkSurfaceFormatKHR>&&		NewFormats)		{ Formats		= std::move(NewFormats); }
		void SetPresentModes(Array<VkPresentModeKHR>&&	NewPresentModes){ PresentModes	= std::move(NewPresentModes); }

		auto GetHandle()		const	-> VkSurfaceKHR	{ return Handle; }	
		operator VkSurfaceKHR() const { return Handle; }

	private:
		VkSurfaceKHR                Handle{ VK_NULL_HANDLE };
		Array<VkSurfaceFormatKHR>	Formats;
		Array<VkPresentModeKHR>		PresentModes;
		VkSurfaceCapabilitiesKHR	Capacities;

		void Create();
		void Destroy();

	public:
		VulkanSurface()  noexcept = default;
		~VulkanSurface() noexcept = default;
	};

	void VulkanSurface::
	Create()
	{
		VK_CHECK(glfwCreateWindowSurface(
			GVulkan->Instance->GetHandle(),
			Platform::GetWindow().GetHandle(),
			GVulkan->AllocationCallbacks,
			&Handle));
	}

	void VulkanSurface::
	Destroy()
	{
		vkDestroySurfaceKHR(GVulkan->Instance->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
} // namespace VE