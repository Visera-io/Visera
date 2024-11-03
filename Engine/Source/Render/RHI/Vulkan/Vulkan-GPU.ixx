module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Render.RHI.Vulkan:GPU;

import Visera.Core.Log;

namespace
{
	
}

export namespace VE
{
	class VulkanContext;

	class VulkanGPU
	{
		friend class VulkanContext;
	public:
		static auto EnumerateAvailableGPUs(VkInstance Instance) -> Array<VulkanGPU>;

		auto GetName()					const -> RawString								{ return Properties.deviceName; }
		auto GetHandle()				const -> VkPhysicalDevice						{ return Handle; }
		auto GetFeatures()				const -> const VkPhysicalDeviceFeatures&		{ return Features; }
		auto GetProperties()			const -> const VkPhysicalDeviceProperties&		{ return Properties; }
		auto GetMemoryProperties()		const -> const VkPhysicalDeviceMemoryProperties&{ return MemoryProperties; }
		auto GetQueueFamilyProperties() const -> const Array<VkQueueFamilyProperties>&	{ return QueueFamilyProperties; }
		auto GetExtensionProperties()	const -> const Array<VkExtensionProperties>&	{ return ExtensionProperties; }
		
		Bool IsDiscreteGPU() const { return Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }

		operator VkPhysicalDevice() const { return Handle; }
		
	private:
		VkPhysicalDevice					Handle { VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures			Features;
		VkPhysicalDeviceProperties			Properties;
		VkPhysicalDeviceMemoryProperties	MemoryProperties;
		Array<VkExtensionProperties>		ExtensionProperties;
		Array<VkQueueFamilyProperties>		QueueFamilyProperties;

	public:
		VulkanGPU() noexcept = default;
		VulkanGPU(VkPhysicalDevice PhysicalDevice) noexcept
			:Handle{PhysicalDevice}
		{
			vkGetPhysicalDeviceFeatures(Handle, &Features);

			vkGetPhysicalDeviceProperties(Handle, &Properties);

			vkGetPhysicalDeviceMemoryProperties(Handle, &MemoryProperties);

			//Queue Families
			UInt32 QueueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(Handle, &QueueFamilyCount, nullptr);
			Assert(QueueFamilyCount != 0);
			QueueFamilyProperties.resize(QueueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(Handle, &QueueFamilyCount, QueueFamilyProperties.data());

			//Extensions
			UInt32 ExtensionCount = 0;
			vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, nullptr);
			Assert(ExtensionCount != 0);
			ExtensionProperties.resize(ExtensionCount);
			vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, ExtensionProperties.data());
		}

		~VulkanGPU()
		{
			
		}
	};

	Array<VulkanGPU> VulkanGPU::
	EnumerateAvailableGPUs(VkInstance Instance)
	{
		/*Find Proper Physical Device(Host)*/
		UInt32 PhysicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, nullptr);
		if (!PhysicalDeviceCount) Log::Fatal("Failed to enumerate GPUs with Vulkan support!");

		Array<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
		vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data());

		Array<VulkanGPU> GPUs(PhysicalDevices.size());
		std::transform(PhysicalDevices.begin(), PhysicalDevices.end(), GPUs.begin(),
			[](VkPhysicalDevice PhysicalDevice) -> VulkanGPU
			{ return VulkanGPU{ PhysicalDevice }; });

		return GPUs;
	}

} // namespace VE