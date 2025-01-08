module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan;

export import :CommandPool;
export import :Shader;
export import :RenderPass;
export import :Synchronization;

import :Enums;
import :Loader;
import :Allocator;
import :PipelineCache;
import :Instance;
import :Device;
import :Surface;
import :Swapchain;

VISERA_PUBLIC_MODULE

#define SI static inline

class Vulkan
{
	friend class RHI;
private:
	using Fence				=VulkanFence;
	using Semaphore			=VulkanSemaphore;
	using CommandPool		=VulkanCommandPool;
	using CommandBuffer		=VulkanCommandPool::CommandBuffer;
	using Shader			=VulkanShader;
	using RenderPass		=VulkanRenderPass;

	using EShaderStage		=EShaderStage;
	using EAccessibility	=EAccessibility;
	using EPipelineStage	=EPipelineStage;
	using EMemoryUsage		=EMemoryUsage;
	using EBufferUsage		=EBufferUsage;
	using EImageLayout		=EImageLayout;
	using EPipelineStage	=EPipelineStage;
	using EAttachmentIO		=EAttachmentIO;

	/* << Vulkan Objects >>*/
	SI VulkanLoader		Loader		{};
	SI VulkanInstance	Instance	{};
	SI VulkanSurface	Surface		{};
	SI VulkanGPU		GPU			{};
	SI VulkanDevice		Device		{};
	SI VulkanAllocator	Allocator	{};
	SI VulkanSwapchain	Swapchain	{};
		
	SI VulkanPipelineCache RenderPassPipelineCache{VISERA_APP_CACHE_DIR "/.RenderPassCache.bin"};
	//VulkanPipelineCache ComputePassPipelineCache	{VISERA_APP_ASSETS_DIR "/.RenderPassCache.bin"};

private:
	SI void Bootstrap();
	SI void Terminate();
};

void Vulkan::
Bootstrap()
{
	GVulkan = new VulkanContext();
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

VISERA_MODULE_END