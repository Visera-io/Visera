module;
#define VOLK_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Loader;
import :Context;
import Visera.Core.Log;

export namespace VE
{

	class FVulkanLoader
	{
		friend class FVulkan;
	private:
		void LoadInstance(VkInstance Instance);
		void LoadDevice(VkDevice Device);

		void Create();
		void Destroy();

	public:
		FVulkanLoader() noexcept = default;
		~FVulkanLoader() noexcept = default;
	};

	void FVulkanLoader::
	Create()
	{
		if (volkInitialize() != VK_SUCCESS)
		{ Log::Fatal("Failed to initialize Volk!"); }
	}

	void FVulkanLoader::
	Destroy()
	{
		volkFinalize();
	}
		
	void FVulkanLoader::
	LoadInstance(VkInstance Instance)
	{
		VE_ASSERT(Instance != VK_NULL_HANDLE);
		volkLoadInstance(Instance);
	}

	void FVulkanLoader::
	LoadDevice(VkDevice Device)
	{
		VE_ASSERT(Device != VK_NULL_HANDLE);
		volkLoadDevice(Device);
	}

} // namespace VE