module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:GPU;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;

import Visera.Core.Log;

export namespace VE
{

	class FVulkanGPU
	{
		friend class FVulkan;
	public:
		auto GetName()				    const -> RawString								{ return Properties.deviceName; }
		auto GetHandle()				const -> VkPhysicalDevice						{ return Handle; }
		auto GetFeatures()				const -> const VkPhysicalDeviceFeatures&		{ return Features; }
		auto GetProperties()			const -> const VkPhysicalDeviceProperties&		{ return Properties; }
		auto GetMemoryProperties()		const -> const VkPhysicalDeviceMemoryProperties&{ return MemoryProperties; }
		auto GetQueueFamilyProperties() const -> const Array<VkQueueFamilyProperties>&	{ return QueueFamilyProperties; }
		auto GetExtensionProperties()	const -> const Array<VkExtensionProperties>&	{ return ExtensionProperties; }

		inline const VkFormatProperties&
		QueryFormatProperties(EVulkanFormat _Format) const;
		inline const Optional<VkImageFormatProperties>&
		QueryTexture2DFormatProperties(EVulkanFormat _Format) const;

		Bool inline
		IsDiscreteGPU() const { return Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }
		Bool inline
		IsTexture2DFormatSupported(EVulkanFormat _Format) const;
		
	private:
		VkPhysicalDevice					Handle { VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures			Features;
		VkPhysicalDeviceProperties			Properties;
		VkPhysicalDeviceMemoryProperties	MemoryProperties;
		Array<VkExtensionProperties>		ExtensionProperties;
		Array<VkQueueFamilyProperties>		QueueFamilyProperties;

		using FFormatPropertyTable = HashMap<EVulkanFormat, VkFormatProperties>;
		mutable FFormatPropertyTable          FormatPropertyTable;

		using FTexture2DFormatPropertyTable = HashMap<EVulkanFormat, Optional<VkImageFormatProperties>>;
		mutable FTexture2DFormatPropertyTable Texture2DFormatPropertyTable;

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

	const VkFormatProperties& FVulkanGPU::
	QueryFormatProperties(EVulkanFormat _Format) const
	{
		if (FormatPropertyTable.contains(_Format))
		{ return FormatPropertyTable[_Format]; }

		auto& NewFormatProperties = FormatPropertyTable[_Format];
		vkGetPhysicalDeviceFormatProperties(Handle, AutoCast(_Format), &NewFormatProperties);
		
		return NewFormatProperties;
	}

	const Optional<VkImageFormatProperties>& FVulkanGPU::
	QueryTexture2DFormatProperties(EVulkanFormat _Format) const
	{
		if(Texture2DFormatPropertyTable.contains(_Format))
		{ return Texture2DFormatPropertyTable[_Format]; }
		
		VkImageFormatProperties ImageFormatProperties{};

		if(vkGetPhysicalDeviceImageFormatProperties(Handle,
			AutoCast(_Format),
			AutoCast(EVulkanImageType::Image2D),
			AutoCast(EVulkanImageTiling::Optimal),
			AutoCast(EVulkanImageUsage::Sampled             |
				     EVulkanImageUsage::TransferDestination |
				     EVulkanImageUsage::InputAttachment),
			0x0,
			&ImageFormatProperties) == VK_SUCCESS)
		{
			return Texture2DFormatPropertyTable[_Format] = ImageFormatProperties;
		}

		return {};
	}

	Bool FVulkanGPU::
	IsTexture2DFormatSupported(EVulkanFormat _Format) const
	{
		return QueryTexture2DFormatProperties(_Format).has_value();
	}

} // namespace VE