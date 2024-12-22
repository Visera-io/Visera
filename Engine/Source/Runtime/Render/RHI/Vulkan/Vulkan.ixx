module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan;
import :Loader;
import :Context;
import :PipelineCache;
export import :Common;
export import :Instance;
export import :Device;
export import :Allocator;
export import :Surface;
export import :Swapchain;
export import :CommandPool;
export import :RenderPass;
export import :Shader;
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
		SI VulkanSurface	Surface		{};
		SI VulkanGPU		GPU			{};
		SI VulkanDevice		Device		{};
		SI VulkanAllocator  Allocator	{};
		SI VulkanSwapchain	Swapchain	{};
		SI VulkanPipelineCache RenderPassPipelineCache	{VISERA_APP_ASSETS_DIR "/.RenderPassCache.bin"};
		//SI VulkanPipelineCache ComputePassPipelineCache	{VISERA_APP_ASSETS_DIR "/.RenderPassCache.bin"};

	private:
		static void Bootstrap();
		static void Terminate();
	};

	void Vulkan::
		Bootstrap()
	{
		auto* Context = const_cast<VulkanContext* const>(GVulkan);
		Context->Instance	= &Instance;
		Context->Surface	= &Surface;
		Context->GPU		= &GPU;
		Context->Device		= &Device;
		Context->Allocator	= &Allocator;
		Context->Swapchain	= &Swapchain;
		Context->RenderPassPipelineCache = &RenderPassPipelineCache;
		
		Loader.Create();
		Loader.LoadInstance(Instance.Create());

		Surface.Create();
		
		Loader.LoadDevice(Device.Create(&GPU, &Surface));
		
		Allocator.Create();

		Swapchain.Create();

		RenderPassPipelineCache.Create();
	}

	void Vulkan::
	Terminate()
	{
		RenderPassPipelineCache.Destroy();

		Swapchain.Destroy();

		Allocator.Destory();

		Device.Destroy();

		Surface.Destroy();

		Instance.Destroy();

		Loader.Destroy();

		delete GVulkan;
	}

} } // namespace VE::Runtime