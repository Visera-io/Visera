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

export namespace VE { namespace Render
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class RHI;

	class VulkanContext
	{
		friend class RHI;
	private:
		/* << Vulkan Objects >>*/
		VulkanLoader		Loader		{};
		VulkanInstance		Instance	{};
		VulkanSurface		Surface		{Instance};
		VulkanDevice		Device		{Instance};
		VulkanSwapchain		Swapchian	{};
		//VulkanMemoryAllocator VMA;

		VulkanCommandPool	ResetableGraphicsCommandPool{Device};

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

		Surface.Create();

		Device.Create(&Surface);
		ResetableGraphicsCommandPool.Create(VulkanDevice::QueueFamilyType::Graphics, VulkanCommandPool::PoolType::Resetable);
	}

	void VulkanContext::
	Destroy()
	{
		ResetableGraphicsCommandPool.Destroy();
		Device.Destroy();

		Surface.Destroy();

		Instance.Destroy();

		Loader.Destroy();
	}

} } // namespace VE::Render