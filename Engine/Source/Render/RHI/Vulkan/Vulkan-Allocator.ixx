module;
#include <ViseraEngine>

#include <vulkan/vulkan.h>

export module Visera.Render.RHI.Vulkan:Allocator;

export namespace VE
{

	class VulkanAllocator
	{
	public:
		static inline const VkAllocationCallbacks* AllocationCallbacks  { nullptr };
	};


} // namespace VE