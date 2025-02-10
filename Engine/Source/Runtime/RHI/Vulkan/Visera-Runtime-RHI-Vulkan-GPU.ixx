module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:GPU;

import :Common;

import Visera.Core.Log;

export namespace VE { namespace Runtime
{

	class FVulkanGPU
	{
		friend class FVulkan;
	public:
		auto GetName()					const -> RawString								{ return Properties.deviceName; }
		auto GetHandle()				const -> const VkPhysicalDevice					{ return Handle; }
		auto GetFeatures()				const -> const VkPhysicalDeviceFeatures&		{ return Features; }
		auto GetProperties()			const -> const VkPhysicalDeviceProperties&		{ return Properties; }
		auto GetMemoryProperties()		const -> const VkPhysicalDeviceMemoryProperties&{ return MemoryProperties; }
		auto GetQueueFamilyProperties() const -> const Array<VkQueueFamilyProperties>&	{ return QueueFamilyProperties; }
		auto GetExtensionProperties()	const -> const Array<VkExtensionProperties>&	{ return ExtensionProperties; }

		auto QueryFormatProperties(EVulkanFormat Format)			const -> VkFormatProperties { VkFormatProperties Properties; vkGetPhysicalDeviceFormatProperties(Handle, AutoCast(Format), &Properties); return Properties; }

		Bool IsDiscreteGPU() const { return Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }
		
	private:
		VkPhysicalDevice					Handle { VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures			Features;
		VkPhysicalDeviceProperties			Properties;
		VkPhysicalDeviceMemoryProperties	MemoryProperties;
		Array<VkExtensionProperties>		ExtensionProperties;
		Array<VkQueueFamilyProperties>		QueueFamilyProperties;

	public:
		FVulkanGPU() noexcept = default;
		FVulkanGPU(VkPhysicalDevice PhysicalDevice) noexcept
			:Handle{PhysicalDevice}
		{
			vkGetPhysicalDeviceFeatures(Handle, &Features);

			vkGetPhysicalDeviceProperties(Handle, &Properties);

			vkGetPhysicalDeviceMemoryProperties(Handle, &MemoryProperties);

			//Queue Families
			UInt32 QueueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(Handle, &QueueFamilyCount, nullptr);
			VE_ASSERT(QueueFamilyCount != 0);
			QueueFamilyProperties.resize(QueueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(Handle, &QueueFamilyCount, QueueFamilyProperties.data());

			//Extensions
			UInt32 ExtensionCount = 0;
			vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, nullptr);
			VE_ASSERT(ExtensionCount != 0);
			ExtensionProperties.resize(ExtensionCount);
			vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, ExtensionProperties.data());
		}

		~FVulkanGPU()
		{
			Handle = VK_NULL_HANDLE;
		}
	};

} } // namespace VE::Runtime