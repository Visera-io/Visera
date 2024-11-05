module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan:Allocator;

export namespace VE { namespace RHI
{

	class VulkanAllocator
	{
	public:
		static inline const VkAllocationCallbacks* AllocationCallbacks  { nullptr };
	};


} } // namespace VE::RHI