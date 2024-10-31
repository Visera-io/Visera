module;
#include <ViseraEngine>

#include <volk.h>
export module Visera.Render.RHI.Vulkan:Device;

import Visera.Core.Log;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanDevice
	{
		friend class VulkanContext;
	private:
		VkDevice            Handle{ VK_NULL_HANDLE };
		operator VkDevice() const { return Handle; }

		struct {
			VkPhysicalDevice Handle { VK_NULL_HANDLE };
			Array<VkQueueFamilyProperties> QueueFamilies;

			operator VkPhysicalDevice() const { return Handle; }
		}Host;

		void Create(VkInstance Instance);
		void Destroy(VkInstance Instance);

	public:
		VulkanDevice() noexcept = default;
		~VulkanDevice() noexcept = default;
	};

	void VulkanDevice::
	Create(VkInstance Instance)
	{
		/*Find Proper Physical Device(Host)*/
		uint32_t GPUCount = 0;
		vkEnumeratePhysicalDevices(Instance, &GPUCount, nullptr);
		if (!GPUCount) Log::Fatal("Failed to enumerate GPUs with Vulkan support!");

		//std::vector<VkPhysicalDevice> physicalDevices(phyDevCnt);
		//vkEnumeratePhysicalDevices(Instance, &phyDevCnt, physicalDevices.data());

		/*UInt32 QueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(Host, &QueueFamilyCount, nullptr);
		Host.QueueFamilies.resize(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Host, &QueueFamilyCount, Host.QueueFamilies.data());*/

		//1. Surface Support
		//vkGetPhysicalDeviceSurfaceSupportKHR(Host, family_idx, surface, &presentSupport);
	}

	void VulkanDevice::
	Destroy(VkInstance Instance)
	{
		
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE