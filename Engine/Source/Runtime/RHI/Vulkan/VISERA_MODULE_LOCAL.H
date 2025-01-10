#pragma once
#include <Visera.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>

namespace VE { namespace Runtime
{
class RHI;
class Vulkan;
class FVulkanInstance;
class FVulkanGPU;
class FVulkanDevice;
class FVulkanAllocator;
class FVulkanSurface;
class FVulkanSwapchain;
class FVulkanPipelineCache;

class VulkanContext
{
	friend class FVulkan;
public:
	const FVulkanInstance*		Instance	{nullptr};
	const FVulkanGPU*			GPU			{nullptr};
	const FVulkanDevice*			Device		{nullptr};
	const FVulkanAllocator*		Allocator	{nullptr};
	const FVulkanSurface*		Surface		{nullptr};
	const FVulkanSwapchain*		Swapchain	{nullptr};
	const FVulkanPipelineCache*	GraphicsPipelineCache	{nullptr};
	const VkAllocationCallbacks*AllocationCallbacks		{nullptr};

	VulkanContext() noexcept { VE_ASSERT(!bCreated); bCreated = true; };
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext(VulkanContext&&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;
	VulkanContext& operator=(VulkanContext&&) = delete;
private:
	static inline bool bCreated = False;
	~VulkanContext() noexcept = default;
};
inline const VulkanContext* GVulkan = nullptr;

} } // namespace VE::Runtime
