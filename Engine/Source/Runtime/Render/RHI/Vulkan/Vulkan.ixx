module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan;
import :Allocator;
import :Loader;
export import :Instance;
export import :Device;
export import :Surface;
export import :Swapchain;
export import :CommandPool;
export import :Synchronization;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }
	#define SI static inline

	class RHI;

	class Vulkan
	{
		friend class RHI;
	private:
		/* << Vulkan Objects >>*/
		SI VulkanLoader		Loader		{};
		SI VulkanInstance	Instance	{};
		SI VulkanSurface	Surface		{Instance};
		SI VulkanDevice		Device		{Instance};
		SI VulkanSwapchain	Swapchain	{Device, Surface};
		//VulkanMemoryAllocator VMA;

	private:
		static void Bootstrap();
		static void Terminate();
	};

	void Vulkan::
	Bootstrap()
	{
		Loader.Create();
		Loader.LoadInstance(Instance.Create());

		Surface.Create();

		Device.Create(&Surface);

		Swapchain.Create();
	}

	void Vulkan::
	Terminate()
	{
		Swapchain.Destroy();

		Device.Destroy();

		Surface.Destroy();

		Instance.Destroy();

		Loader.Destroy();
	}

} } // namespace VE::Runtime