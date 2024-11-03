module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Render.RHI.Vulkan;
import :Loader;
export import :Instance;
export import :Device;
export import :Surface;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class RHI;

	class VulkanContext
	{
		friend class RHI;
	private:
		/* << Vulkan Objects >>*/
		VulkanLoader   Loader	{};
		VulkanInstance Instance	{};
		VulkanSurface  Surface	{};
		VulkanDevice   Device	{};
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

} // namespace VE