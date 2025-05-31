module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Core:GPU;
#define VE_MODULE_NAME "RHI.Core:GPU"

import :Common;

export namespace VE
{

	class FGPU
	{
    public:
		[[nodiscard]] RawString
		GetName()					const{ return Properties.deviceName; }
		[[nodiscard]] VkPhysicalDevice
		GetHandle()					const{ return Handle; }
		[[nodiscard]] const VkPhysicalDeviceFeatures&
		GetFeatures()				const { return Features; }
		[[nodiscard]] const VkPhysicalDeviceProperties&
		GetProperties()				const { return Properties; }
		[[nodiscard]] const VkPhysicalDeviceMemoryProperties&
		GetMemoryProperties()		const { return MemoryProperties; }
		[[nodiscard]] const Array<VkQueueFamilyProperties>&
		GetQueueFamilyProperties()	const { return QueueFamilyProperties; }
		[[nodiscard]] const Array<VkExtensionProperties>&
		GetExtensionProperties()	const { return ExtensionProperties; }

		[[nodiscard]] Bool inline
		IsDiscreteGPU() const { return Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }
		[[nodiscard]] Bool inline
		IsValidTextureFormat(EVkImageType _Type, EVkFormat _Format) const;

		[[nodiscard]] inline const VkFormatProperties&
		QueryFormatProperties(EVkFormat _Format) const;
		[[nodiscard]] inline const Optional<VkImageFormatProperties>&
		QueryTextureFormatProperties(EVkImageType _Type, EVkFormat _Format) const;
	
	private:
		VkPhysicalDevice					Handle { VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures			Features;
		VkPhysicalDeviceProperties			Properties;
		VkPhysicalDeviceMemoryProperties	MemoryProperties;
		Array<VkExtensionProperties>		ExtensionProperties;
		Array<VkQueueFamilyProperties>		QueueFamilyProperties;

		using FFormatPropertyTable = HashMap<EVkFormat, VkFormatProperties>;
		mutable FFormatPropertyTable          FormatPropertyTable;

		using FTexture2DFormatPropertyTable = HashMap<EVkFormat, Optional<VkImageFormatProperties>>;
		mutable FTexture2DFormatPropertyTable Texture2DFormatPropertyTable;

	public:
		FGPU() noexcept = default;
		FGPU(VkPhysicalDevice PhysicalDevice) noexcept
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

		~FGPU()
		{
			Handle = VK_NULL_HANDLE;
		}
	};

	const VkFormatProperties& FGPU::
	QueryFormatProperties(EVkFormat _Format) const
	{
		if (FormatPropertyTable.contains(_Format))
		{ return FormatPropertyTable[_Format]; }

		auto& NewFormatProperties = FormatPropertyTable[_Format];
		vkGetPhysicalDeviceFormatProperties(Handle, AutoCast(_Format), &NewFormatProperties);
		
		return NewFormatProperties;
	}

	const Optional<VkImageFormatProperties>& FGPU::
	QueryTextureFormatProperties(EVkImageType _Type, EVkFormat _Format) const
	{
		if(Texture2DFormatPropertyTable.contains(_Format))
		{ return Texture2DFormatPropertyTable[_Format]; }
		
		VkImageFormatProperties ImageFormatProperties{};

		if(vkGetPhysicalDeviceImageFormatProperties(Handle,
			AutoCast(_Format),
			AutoCast(_Type),
			AutoCast(EVkImageTiling::Optimal),
			AutoCast(EVkImageUsage::Sampled             |
				     EVkImageUsage::TransferDestination |
				     EVkImageUsage::InputAttachment),
			0x0,
			&ImageFormatProperties) == VK_SUCCESS)
		{
			return Texture2DFormatPropertyTable[_Format] = ImageFormatProperties;
		}

		return {};
	}

	Bool FGPU::
	IsValidTextureFormat(EVkImageType _Type, EVkFormat _Format) const
	{
		return QueryTextureFormatProperties(_Type, _Format).has_value();
	}

} // namespace VE