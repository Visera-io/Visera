module;
#include <Visera>

#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Surface;

import Visera.Engine.Runtime.Platform;
import :Allocator;
import :Instance;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanSurface
	{
		friend class Vulkan;
	public:
		auto GetHandle()		const	-> VkSurfaceKHR						{ return Handle; }
		auto GetFormats()		const	-> const Array<VkSurfaceFormatKHR>& { return Formats; }
		auto GetPresentModes()	const	-> const Array<VkPresentModeKHR>&	{ return PresentModes; }
		void SetFormats(Array<VkSurfaceFormatKHR>&&		NewFormats)		{ Formats		= std::move(NewFormats); }
		void SetPresentModes(Array<VkPresentModeKHR>&&	NewPresentModes){ PresentModes	= std::move(NewPresentModes); }
		operator VkSurfaceKHR() const { return Handle; }

	private:
		VkSurfaceKHR                Handle{ VK_NULL_HANDLE };
		const VulkanInstance&		HostInstance;
		Array<VkSurfaceFormatKHR>	Formats;
		Array<VkPresentModeKHR>		PresentModes;

		void Create();
		void Destroy();

	public:
		VulkanSurface(const VulkanInstance& Instance) noexcept :HostInstance{ Instance } {};
		VulkanSurface() noexcept = delete;
		~VulkanSurface() noexcept = default;
	};

	void VulkanSurface::
	Create()
	{
		Assert(HostInstance.GetHandle() != VK_NULL_HANDLE);
		VK_CHECK(glfwCreateWindowSurface(
			HostInstance.GetHandle(),
			Platform::GetWindow().GetHandle(),
			VulkanAllocator::AllocationCallbacks,
			&Handle));
	}

	void VulkanSurface::
	Destroy()
	{
		Assert(HostInstance.GetHandle() != VK_NULL_HANDLE);
		vkDestroySurfaceKHR(HostInstance.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
} } // namespace VE::Runtime