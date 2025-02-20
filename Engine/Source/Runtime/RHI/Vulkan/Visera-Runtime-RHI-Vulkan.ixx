module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan;
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

	class RHI;

	class FVulkan
	{
		friend class RHI;
	public:
		auto GetInstance()		const -> const FVulkanInstance& { return Instance; }
		auto GetSurface()		const -> const FVulkanSurface&  { return Surface; }
		auto GetSwapchain()		const -> const FVulkanSwapchain&{ return Swapchain; }
		auto GetGPU()			const -> const FVulkanGPU&		{ return GPU; }
		auto GetDevice()		const -> const FVulkanDevice&	{ return Device; }
		auto GetGraphicsPipelineCache() const -> const FVulkanPipelineCache& { return GraphicsPipelineCache; }
		auto GetAllocationCallbacks()	const -> const VulkanAllocationCallbacks { return AllocationCallbacks; }
		
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

} // namespace VE