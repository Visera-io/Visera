module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Context;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;
	class VulkanInstance;
	class VulkanGPU;
	class VulkanDevice;
	class VulkanSurface;
	class VulkanSwapchain;
	class VulkanPipelineCache;

	class VulkanContext
	{
		friend class Vulkan;
	public:
		const VulkanInstance*	Instance;
		const VulkanGPU*		GPU;
		const VulkanDevice*		Device;
		const VulkanSurface*	Surface;
		const VulkanSwapchain*	Swapchain;
		const VulkanPipelineCache* RenderPassPipelineCache;
		const VkAllocationCallbacks* AllocationCallbacks = nullptr;

		VulkanContext() noexcept { Assert(!bCreated); bCreated = true; };
		VulkanContext(const VulkanContext&) = delete;
		VulkanContext(VulkanContext&&) = delete;
		VulkanContext& operator=(const VulkanContext&) = delete;
		VulkanContext& operator=(VulkanContext&&) = delete;
	private:
		static inline bool bCreated = False;
		~VulkanContext() noexcept = default;
	};
	inline const VulkanContext* const GVulkan = new VulkanContext();

} } // namespace VE::Runtime