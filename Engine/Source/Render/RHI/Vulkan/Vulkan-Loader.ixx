module;
#include <ViseraEngine>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk.h>

export module Visera.Render.RHI.Vulkan:Loader;

import Visera.Core.Base.Exception;

export namespace VE
{

	class VulkanLoader
	{
	public:
		struct CreateInfo
		{
			VkInstance App;
			VkDevice   Device;
			Bool IsValid() const { return App != VK_NULL_HANDLE && Device != VK_NULL_HANDLE; }
		};

		VulkanLoader() = delete;
		VulkanLoader(const CreateInfo& createinfo)
		{
			if (VK_SUCCESS != volkInitialize())
			{ throw RuntimeError("Failed to initialize Volk!"); }

			Assert(createinfo.IsValid());
			volkLoadInstance(createinfo.App);
			volkLoadDevice(createinfo.Device);
		}

	};

} // namespace VE