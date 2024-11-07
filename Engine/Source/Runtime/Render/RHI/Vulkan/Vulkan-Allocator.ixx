module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Allocator;

export namespace VE { namespace Runtime
{

	class VulkanAllocator
	{
	public:
		static inline const VkAllocationCallbacks* AllocationCallbacks  { nullptr };
	};


} } // namespace VE::Runtime