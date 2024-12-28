module;
#include "VulkanPC.h"
export module Visera.Runtime.Render.RHI.Vulkan;

import :Loader;
import :PipelineCache;
export import :Instance;
export import :Device;
export import :Allocator;
export import :Surface;
export import :Swapchain;
export import :CommandPool;
export import :RenderPass;
export import :Shader;
export import :Synchronization;

export namespace VE
{
	#define SI static inline

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

} // namespace VE