module;
#include <Visera>

#include <volk.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Allocator;

import :Context;
import :Instance;
import :Device;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanAllocator
	{
		friend class Vulkan;
	public:
		auto GetHandle() const -> VmaAllocator { return Handle; }
		operator VmaAllocator() const { return Handle; }
	
	private:
		void Create()
		{
			VmaVulkanFunctions VulkanFunctions
			{
				.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
				.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
				.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
				.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements
			};

			VmaAllocatorCreateInfo CreateInfo
			{
				.physicalDevice = GVulkan->GPU->GetHandle(),
				.device = GVulkan->Device->GetHandle(),
				.pVulkanFunctions = &VulkanFunctions,
				.instance = GVulkan->Instance->GetHandle(),
				.vulkanApiVersion = GVulkan->Instance->GetVulkanAPIVersion()
			};
			VK_CHECK(vmaCreateAllocator(&CreateInfo, &Handle));
		}

		void Destory()
		{
			vmaDestroyAllocator(Handle);
			Handle = VK_NULL_HANDLE;
		}

	private:
		VmaAllocator	Handle{ VK_NULL_HANDLE };
	};


} } // namespace VE::Runtime