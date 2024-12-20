module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Allocator;

import :Context;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanAllocator
	{
		friend class Vulkan;
	public:
		static inline const VkAllocationCallbacks* AllocationCallbacks  { nullptr };
	};


} } // namespace VE::Runtime