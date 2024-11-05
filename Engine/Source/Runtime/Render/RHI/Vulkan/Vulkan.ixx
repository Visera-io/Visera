module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan;
import :Allocator;
import :Loader;
export import :Instance;
export import :Device;
export import :Surface;
export import :Swapchain;
export import :CommandPool;

export namespace VE { namespace RHI
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Layer;

	class VulkanContext
	{
		friend class Layer;
	private:
		/* << Vulkan Objects >>*/
		VulkanLoader		Loader		{};
		VulkanInstance		Instance	{};
		VulkanSurface		Surface		{};
		VulkanDevice		Device		{};
		VulkanSwapchain		Swapchian	{};
		//VulkanMemoryAllocator VMA;

	private:
		void Create();
		void Destroy();

	public:
		VulkanContext() noexcept = default;
		~VulkanContext() noexcept = default;
	};

	void VulkanContext::
	Create()
	{
		Loader.Create();
		Loader.LoadInstance(Instance.Create());

		Surface.Create(Instance);

		Device.Create(Instance, &Surface);
	}

	void VulkanContext::
	Destroy()
	{
		Device.Destroy(Instance);

		Surface.Destroy(Instance);

		Instance.Destroy();

		Loader.Destroy();
	}

} } // namespace VE::RHI