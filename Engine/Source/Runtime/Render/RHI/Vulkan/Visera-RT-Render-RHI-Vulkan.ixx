module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan;
#define VE_MODULE_NAME "Vulkan"
export import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Context;
export import :CommandPool;
export import :CommandBuffer;
export import :DescriptorPool;
export import :DescriptorSet;
export import :DescriptorSetLayout;
export import :Shader;
export import :Sampler;
export import :PipelineLayout;
export import :RenderPass;
export import :RenderPassLayout;
export import :RenderPipeline;
export import :RenderPipelineSetting;
export import :RenderTarget;
export import :Framebuffer;
export import :Synchronization;
export import :Loader;
export import :Allocator;
export import :PipelineCache;
export import :Instance;
export import :Device;
export import :GPU;
export import :Surface;
export import :Swapchain;

import Visera.Core.Log;

export namespace VE
{
	using VulkanAllocationCallbacks = VkAllocationCallbacks*;

	class FVulkan
	{
	public:
		auto GetAllocator()	 -> FVulkanAllocator&{ return Allocator; }
		auto GetInstance()	 -> FVulkanInstance& { return Instance; }
		auto GetSurface()	 -> FVulkanSurface&  { return Surface; }
		auto GetSwapchain()	 -> FVulkanSwapchain&{ return Swapchain; }
		auto GetGPU()		 -> FVulkanGPU&		{ return GPU; }
		auto GetDevice()	 -> FVulkanDevice&	{ return Device; }
		auto GetGraphicsPipelineCache() -> FVulkanPipelineCache& { return GraphicsPipelineCache; }
		auto GetAllocationCallbacks()   -> VulkanAllocationCallbacks { return AllocationCallbacks; }

	private:
		/* << Vulkan Objects >>*/
		FVulkanLoader		Loader		{};
		FVulkanInstance		Instance	{VK_API_VERSION_1_3};
		FVulkanSurface		Surface		{};
		FVulkanGPU			GPU			{};
		FVulkanDevice		Device		{};
		FVulkanAllocator	Allocator	{};
		FVulkanSwapchain	Swapchain	{};

		FVulkanPipelineCache GraphicsPipelineCache{FPath{ StringView(VISERA_APP_CACHE_DIR"/.GraphicsPipelineCache.bin") }};
		//VulkanPipelineCache ComputeCache	{VISERA_APP_ASSETS_DIR "/.ComputeCache.bin"};

		VulkanAllocationCallbacks AllocationCallbacks {nullptr};

		FVulkanGraphicsCommandPool* ResetableGraphicsCommandPool;
		FVulkanGraphicsCommandPool* TransientGraphicsCommandPool;

	public:
		FVulkan();
		~FVulkan();
	};

	FVulkan::
	FVulkan()
	{
		VE_LOG_TRACE("Creating Vulkan Context...");
		GVulkan = new FVulkanContext();
		auto* Context = const_cast<FVulkanContext* const>(GVulkan);
		Context->Instance	= &Instance;
		Context->Surface	= &Surface;
		Context->GPU		= &GPU;
		Context->Device		= &Device;
		Context->Allocator	= &Allocator;
		Context->Swapchain	= &Swapchain;
		Context->GraphicsPipelineCache = &GraphicsPipelineCache;
		Context->AllocationCallbacks   = AllocationCallbacks;
		
		VE_LOG_TRACE("Creating Vulkan Loader...");
		Loader.Create();
		VE_LOG_TRACE("Creating Vulkan Instance...");
		Loader.LoadInstance(Instance.Create());
		VE_LOG_TRACE("Creating Vulkan Surface...");
		Surface.Create();
		VE_LOG_TRACE("Creating Vulkan Device...");
		Loader.LoadDevice(Device.Create(&GPU, &Surface));
		VE_LOG_TRACE("Creating Vulkan Memory Allcator (VMA)...");
		Allocator.Create();
		VE_LOG_TRACE("Creating Vulkan Swapchain...");
		Swapchain.Create();
		VE_LOG_TRACE("Creating Vulkan Graphics Pipeline Cache...");
		GraphicsPipelineCache.Create();
	}

	FVulkan::
	~FVulkan()
	{
		VE_LOG_TRACE("Destroying Vulkan Graphics Pipeline Cache...");
		GraphicsPipelineCache.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Swapchain...");
		Swapchain.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Memory Allcator (VMA)...");
		Allocator.Destory();
		VE_LOG_TRACE("Destroying Vulkan Device...");
		Device.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Surface...");
		Surface.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Instance...");
		Instance.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Loader...");
		Loader.Destroy();
		VE_LOG_TRACE("Destroying Vulkan Context...");
		delete GVulkan;
	}

} // namespace VE