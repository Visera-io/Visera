module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Surface;

import Visera.Runtime.Window;
import Visera.Core.Signal;

import :Instance;
import :GPU;

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
		{ throw SRuntimeError("Failed to create Vulkan Window Surface!"); }
	}

	void FVulkanSurface::
	Destroy()
	{
		vkDestroySurfaceKHR(GVulkan->Instance->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE