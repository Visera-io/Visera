module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Context;

export namespace VE
{
    class FVulkan;
    class FVulkanInstance;
    class FVulkanGPU;
    class FVulkanDevice;
    class FVulkanAllocator;
    class FVulkanSurface;
    class FVulkanSwapchain;
    class FVulkanPipelineCache;

    class FVulkanContext
    {
        friend class FVulkan;
    public:
        const FVulkanInstance*		Instance	{nullptr};
        const FVulkanGPU*			GPU			{nullptr};
        const FVulkanDevice*		Device		{nullptr};
        const FVulkanAllocator*		Allocator	{nullptr};
        const FVulkanSurface*		Surface		{nullptr};
        const FVulkanSwapchain*		Swapchain	{nullptr};
        const FVulkanPipelineCache*	GraphicsPipelineCache	{nullptr};
        const VkAllocationCallbacks*AllocationCallbacks		{nullptr};

        FVulkanContext() noexcept { VE_ASSERT(!bCreated); bCreated = true; };
        FVulkanContext(const FVulkanContext&) = delete;
        FVulkanContext(FVulkanContext&&) = delete;
        FVulkanContext& operator=(const FVulkanContext&) = delete;
        FVulkanContext& operator=(FVulkanContext&&) = delete;
    private:
        static inline bool bCreated = False;
        ~FVulkanContext() noexcept = default;
    };
    inline const FVulkanContext* GVulkan = nullptr;

} // namespace VE
