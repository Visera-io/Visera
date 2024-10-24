module;
#include <ViseraEngine>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk.h>

export module Visera.Render.RHI.Vulkan:Loader;

import Visera.Core.Log;

export namespace VE
{
	class Vulkan;

	class VulkanLoader
	{
		friend class Vulkan;
	private:
		VulkanLoader() noexcept = default;

		static inline void
		Bootstrap()
		{ 
			if (!VK_SUCCESS == volkInitialize())
			{ Log::Fatal("Failed to initialize Volk!"); }
		};

		static inline void
		Terminate()
		{
			volkFinalize();
		}
		
		void
		LoadInstance(VkInstance instance)
		{
			Assert(instance != VK_NULL_HANDLE);
			volkLoadInstance(instance);
		}

		void
		LoadDevice(VkDevice device)
		{
			Assert(device != VK_NULL_HANDLE);
			volkLoadDevice(device);
		}

	};

} // namespace VE