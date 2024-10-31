module;
#include <ViseraEngine>

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
		UniquePtr<VulkanLoader>   Loader	= CreateUniquePtr<VulkanLoader>();
		UniquePtr<VulkanInstance> Instance	= CreateUniquePtr<VulkanInstance>();
		UniquePtr<VulkanSurface>  Surface	= CreateUniquePtr<VulkanSurface>();
		UniquePtr<VulkanDevice>   Device	= CreateUniquePtr<VulkanDevice>();
		//UniquePtr<VulkanMemoryAllocator> VMA;

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
		Loader->Create();
		Loader->LoadInstance(Instance->Create());

		Surface->Create(Instance->Handle);

		Device->Create(Instance->Handle);
	}

	void VulkanContext::
	Destroy()
	{
		Device->Destroy(Instance->Handle);

		Surface->Destroy(Instance->Handle);

		Instance->Destroy();

		Loader->Destroy();
	}

} // namespace VE