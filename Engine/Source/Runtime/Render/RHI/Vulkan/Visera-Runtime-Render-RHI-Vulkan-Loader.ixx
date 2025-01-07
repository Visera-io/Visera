module;
#define VOLK_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Loader;

import Visera.Core.Log;

VISERA_PUBLIC_MODULE

class VulkanLoader
{
	friend class Vulkan;
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
	if (VK_SUCCESS != volkInitialize())
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
	VE_ASSERT(Instance != VK_NULL_HANDLE);
	volkLoadInstance(Instance);
}

void VulkanLoader::
LoadDevice(VkDevice Device)
{
	VE_ASSERT(Device != VK_NULL_HANDLE);
	volkLoadDevice(Device);
}

VISERA_MODULE_END