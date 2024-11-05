module;
#include <ViseraEngine>

#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION
#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan:Loader;

import Visera.Core.Log;

export namespace VE { namespace RHI
{

	class VulkanContext;

	class VulkanLoader
	{
		friend class VulkanContext;
	private:
		void LoadInstance(VkInstance Instance);
		void LoadDevice(VkDevice Device);

		void Create();
		void Destroy();

	public:
		VulkanLoader() noexcept = default;
		~VulkanLoader() noexcept = default;
	};

	void VulkanLoader::
	Create()
	{
		if (!VK_SUCCESS == volkInitialize())
		{ Log::Fatal("Failed to initialize Volk!"); }
	}

	void VulkanLoader::
	Destroy()
	{
		volkFinalize();
	}
		
	void VulkanLoader::
	LoadInstance(VkInstance Instance)
	{
		Assert(Instance != VK_NULL_HANDLE);
		volkLoadInstance(Instance);
	}

	void VulkanLoader::
	LoadDevice(VkDevice Device)
	{
		Assert(Device != VK_NULL_HANDLE);
		volkLoadDevice(Device);
	}

} } // namespace VE::RHI