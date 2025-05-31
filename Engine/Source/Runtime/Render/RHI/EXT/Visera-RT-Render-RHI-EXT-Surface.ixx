module;
#include <Visera.h>
#include "../Core/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.EXT:Surface;
#define VE_MODULE_NAME "RHI.EXT:Surface"

import Visera.Runtime.Render.RHI.Core;

import Visera.Runtime.Platform.Window;
import Visera.Core.Log;

export namespace VE
{

	class FVulkanSurface
	{
	public:
		[[nodiscard]] auto
		Create(SharedPtr<FInstance> _Instance) { return CreateUniquePtr<FVulkanSurface>(_Instance); }

		auto GetFormats()		const	-> const Array<VkSurfaceFormatKHR>& { return Formats; }
		auto GetPresentModes()	const	-> const Array<VkPresentModeKHR>&	{ return PresentModes; }

		auto GetHandle()		const	-> VkSurfaceKHR	{ return Handle; }	

	private:
		WeakPtr<FInstance>          Host;
		VkSurfaceKHR                Handle{ VK_NULL_HANDLE };

		Array<VkSurfaceFormatKHR>	Formats;
		Array<VkPresentModeKHR>		PresentModes;
		VkSurfaceCapabilitiesKHR	Capacities;

	public:
		FVulkanSurface() = delete;
		FVulkanSurface(SharedPtr<FInstance> _Instance)  noexcept;
		~FVulkanSurface() noexcept;
	};

	FVulkanSurface::
	FVulkanSurface(SharedPtr<FInstance> _Instance)
		:Host { _Instance }
	{
		VE_ASSERT(!Host.expired());
		auto Instance = Host.lock();

		if(glfwCreateWindowSurface(
			Instance->GetHandle(),
			Window::GetHandle(),
			AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to create Vulkan Window Surface!"); }
	}

	FVulkanSurface::
	~FVulkanSurface()
	{
		VE_ASSERT(!Host.expired());
		auto Instance = Host.lock();

		vkDestroySurfaceKHR(Instance->GetHandle(), Handle, AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE