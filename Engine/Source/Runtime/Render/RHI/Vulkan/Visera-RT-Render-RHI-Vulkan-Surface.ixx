module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Surface;
#define VE_MODULE_NAME "Vulkan:Surface"
import :Context;
import :Instance;
import :GPU;

import Visera.Runtime.Platform.Window;
import Visera.Core.Log;

export namespace VE
{

	class FVulkanSurface
	{
		friend class FVulkan;
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
		FVulkanSurface()  noexcept = default;
		~FVulkanSurface() noexcept = default;
	};

	void FVulkanSurface::
	Create()
	{
		if(VK_SUCCESS != glfwCreateWindowSurface(
			GVulkan->Instance->GetHandle(),
			Window::GetHandle(),
			GVulkan->AllocationCallbacks,
			&Handle))
		{ VE_LOG_FATAL("Failed to create Vulkan Window Surface!"); }
	}

	void FVulkanSurface::
	Destroy()
	{
		vkDestroySurfaceKHR(GVulkan->Instance->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE