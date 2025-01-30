module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan;

export import :CommandPool;
export import :Shader;
export import :PipelineLayout;
export import :RenderPass;
export import :RenderPipeline;
export import :Synchronization;
export import :Enums;
export import :Loader;
export import :Allocator;
export import :PipelineCache;
export import :Framebuffer;
export import :Instance;
export import :Device;
export import :GPU;
export import :Surface;
export import :Swapchain;

export namespace VE { namespace Runtime
{
	using VulkanAllocationCallbacks = VkAllocationCallbacks*;

	class RHI;
	class UI;

	class FVulkan
	{
		friend class RHI;
		friend class UI;
	private:
		/* << Vulkan Objects >>*/
		FVulkanLoader		Loader		{};
		FVulkanInstance		Instance	{};
		FVulkanSurface		Surface		{};
		FVulkanGPU			GPU			{};
		FVulkanDevice		Device		{};
		FVulkanAllocator	Allocator	{};
		FVulkanSwapchain	Swapchain	{};

		FVulkanPipelineCache GraphicsPipelineCache{VISERA_APP_CACHE_DIR "/.GraphicsPipelineCache.bin"};
		//VulkanPipelineCache ComputeCache	{VISERA_APP_ASSETS_DIR "/.ComputeCache.bin"};

		VulkanAllocationCallbacks AllocationCallbacks {nullptr};
	
		FVulkan();
		~FVulkan();
	};

	FVulkan::
	FVulkan()
	{
		GVulkan = new VulkanContext();
		auto* Context = const_cast<VulkanContext* const>(GVulkan);
		Context->Instance	= &Instance;
		Context->Surface	= &Surface;
		Context->GPU		= &GPU;
		Context->Device		= &Device;
		Context->Allocator	= &Allocator;
		Context->Swapchain	= &Swapchain;
		Context->GraphicsPipelineCache = &GraphicsPipelineCache;
		Context->AllocationCallbacks   = AllocationCallbacks;
		
		Loader.Create();
		Loader.LoadInstance(Instance.Create());

		Surface.Create();
		
		Loader.LoadDevice(Device.Create(&GPU, &Surface));
		
		Allocator.Create();

		Swapchain.Create();

		GraphicsPipelineCache.Create();
	}

	FVulkan::
	~FVulkan()
	{
		GraphicsPipelineCache.Destroy();

		Swapchain.Destroy();

		Allocator.Destory();

		Device.Destroy();

		Surface.Destroy();

		Instance.Destroy();

		Loader.Destroy();

		delete GVulkan;
	}

} } // namespace VE::Runtime