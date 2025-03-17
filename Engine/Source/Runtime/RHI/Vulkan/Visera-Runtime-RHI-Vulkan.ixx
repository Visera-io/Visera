module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan;
import :Context;
export import :CommandPool;
export import :CommandBuffer;
export import :DescriptorPool;
export import :DescriptorSet;
export import :DescriptorSetLayout;
export import :Shader;
export import :PipelineLayout;
export import :RenderPass;
export import :RenderPassLayout;
export import :RenderPipeline;
export import :RenderPipelineSetting;
export import :RenderTarget;
export import :Framebuffer;
export import :Synchronization;
export import :Common;
export import :Loader;
export import :Allocator;
export import :PipelineCache;
export import :Instance;
export import :Device;
export import :GPU;
export import :Surface;
export import :Swapchain;

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

} // namespace VE