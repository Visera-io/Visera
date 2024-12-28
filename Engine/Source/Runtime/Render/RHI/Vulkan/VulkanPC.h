#pragma once
#include <Visera.h>

#define VK_NO_PROTOTYPES
#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>

#define VK_CHECK(Func) { if (VK_SUCCESS != Func) VE_ASSERT(False); }

VISERA_PRIVATE_MODULE

class RHI;
class Vulkan;
class VulkanInstance;
class VulkanGPU;
class VulkanDevice;
class VulkanAllocator;
class VulkanSurface;
class VulkanSwapchain;
class VulkanPipelineCache;

class VulkanContext
{
	friend class Vulkan;
public:
	const VulkanInstance*		Instance	{nullptr};
	const VulkanGPU*			GPU			{nullptr};
	const VulkanDevice*			Device		{nullptr};
	const VulkanAllocator*		Allocator	{nullptr};
	const VulkanSurface*		Surface		{nullptr};
	const VulkanSwapchain*		Swapchain	{nullptr};
	const VulkanPipelineCache*	RenderPassPipelineCache	{nullptr};
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

VISERA_MODULE_END
