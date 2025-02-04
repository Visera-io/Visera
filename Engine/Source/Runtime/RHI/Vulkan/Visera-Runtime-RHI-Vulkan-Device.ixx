module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Device;

import :Common;
import :Instance;
import :GPU;
import :Surface;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

class FVulkanDevice
{
	friend class FVulkan;
public:
	void WaitIdle()  const { vkDeviceWaitIdle(Handle); }
	auto GetHandle() const -> const VkDevice { return Handle; }

private:
	VkDevice				Handle{ VK_NULL_HANDLE };
	Array<RawString>		Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME,
										VK_KHR_MAINTENANCE1_EXTENSION_NAME };
public:
	struct FQueueFamily
	{
		UInt32				Index = UINT32_MAX;
		Array<VkQueue>		Queues;
		Array<Float>		QueuePriorities{ 1.0 };
		Bool IsValid() const { return Index != UINT32_MAX; }
	};
	auto GetQueueFamily(EQueueFamily Type) const -> const FQueueFamily& { return QueueFamilies[AutoCast(Type)]; }

private:
	Array<FQueueFamily> QueueFamilies;

	auto Create(FVulkanGPU* GPU, FVulkanSurface* Surface) -> VkDevice;
	void Destroy();

	FVulkanDevice()	noexcept	= default;
	~FVulkanDevice() noexcept	= default;
};

VkDevice FVulkanDevice::
Create(FVulkanGPU* GPU, FVulkanSurface* Surface)
{
	//Find Suitable Host GPU
	auto GPUs = GVulkan->Instance->EnumerateAvailableGPUs();

	for (const auto& GPUCandidate : GPUs)
	{
		//Features
		if (!GPUCandidate.IsDiscreteGPU() ||
			!GPUCandidate.GetFeatures().samplerAnisotropy)
		{ continue; }

		//Queue Families Properties
		QueueFamilies.resize(AutoCast(EQueueFamily::All));
		{
			const auto& Properties = GPUCandidate.GetQueueFamilyProperties();
			Set<UInt32>		GraphicsQueueFamilies;
			Set<UInt32>		PresentQueueFamilies;
			Array<UInt32>	TransferQueueFamilies;
			Array<UInt32>	ComputeQueueFamilies;
			for (UInt32 Index = 0; Index < Properties.size(); ++Index)
			{
				if(VK_QUEUE_GRAPHICS_BIT & Properties[Index].queueFlags)
				{ GraphicsQueueFamilies.emplace(Index); }

				VkBool32 bPresentSupported = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(GPUCandidate.GetHandle(), Index, Surface->GetHandle(), &bPresentSupported);
				if (bPresentSupported)
				{ PresentQueueFamilies.emplace(Index); }

				if(VK_QUEUE_TRANSFER_BIT & Properties[Index].queueFlags)
				{ TransferQueueFamilies.emplace_back(Index); }

				if(VK_QUEUE_COMPUTE_BIT & Properties[Index].queueFlags)
				{ ComputeQueueFamilies.emplace_back(Index); }
			}
			//Graphics and Present in the same queue family
			Array<UInt32> GraphicsAndPresentQueueFamilies;
			std::set_intersection(GraphicsQueueFamilies.begin(), GraphicsQueueFamilies.end(),
									PresentQueueFamilies.begin(),  PresentQueueFamilies.end(),
									std::back_inserter(GraphicsAndPresentQueueFamilies));
			if(GraphicsAndPresentQueueFamilies.empty())
			{ throw SRuntimeError("Failed to find a queue family supporting both Graphics and Present!"); }

			Bool Found = False;
			for (UInt32 IdxA : GraphicsAndPresentQueueFamilies)
			{
				QueueFamilies[AutoCast(EQueueFamily::Graphics)].Index = IdxA;
				QueueFamilies[AutoCast(EQueueFamily::Present)].Index = IdxA;
				for (UInt32 IdxB : TransferQueueFamilies)
				{
					if (IdxB == IdxA) continue;
					QueueFamilies[AutoCast(EQueueFamily::Transfer)].Index = IdxB;
					for (UInt32 IdxC : ComputeQueueFamilies)
					{
						if (IdxC == IdxB) continue;
						QueueFamilies[AutoCast(EQueueFamily::Compute)].Index = IdxC;
						Found = True;
					}
					if (Found) break;
				}
				if (Found) break;
			}
			if (!Found) throw SRuntimeError("Failed to find queue families that fit all requirements!");
		}

		//Extension Supports
		{
			Set<StringView> HostExtensionNames;
			std::transform(GPUCandidate.GetExtensionProperties().begin(), GPUCandidate.GetExtensionProperties().end(),
							std::inserter(HostExtensionNames, HostExtensionNames.end()),
							[](const VkExtensionProperties& Property) {return Property.extensionName; });

			UInt32 FoundCount = 0;
			for (const auto& RequiredExtension : Extensions)
			{
				if (HostExtensionNames.count(RequiredExtension)) FoundCount++;
			}
			if (FoundCount < Extensions.size()) continue;
		}

		//Surface Supports
		{
			UInt32 FormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(GPUCandidate.GetHandle(), Surface->GetHandle(), &FormatCount, nullptr);
			if (!FormatCount) continue;
			Array<VkSurfaceFormatKHR> Formats(FormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(GPUCandidate.GetHandle(), Surface->GetHandle(), &FormatCount, Formats.data());

			UInt32 PresentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(GPUCandidate.GetHandle(), Surface->GetHandle(), &PresentModeCount, nullptr);
			if (!PresentModeCount) continue;
			Array<VkPresentModeKHR> PresentModes(PresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(GPUCandidate.GetHandle(), Surface->GetHandle(), &PresentModeCount, PresentModes.data());
			Surface->SetFormats(std::move(Formats));
			Surface->SetPresentModes(std::move(PresentModes));
		}
			
		//Found Suitable Host GPU
		{ *GPU = std::move(GPUCandidate); break; }
	}
	if (GVulkan->GPU->GetHandle() == VK_NULL_HANDLE)
	{ throw SRuntimeError("Failed to find a suitable Physical Device on current computer!"); }

	auto& GraphicsQueueFamily	= QueueFamilies[AutoCast(EQueueFamily::Graphics)];
	auto& PresentQueueFamily	= QueueFamilies[AutoCast(EQueueFamily::Present)];
	auto& TransferQueueFamily	= QueueFamilies[AutoCast(EQueueFamily::Transfer)];
	auto& ComputeQueueFamily	= QueueFamilies[AutoCast(EQueueFamily::Compute)];

	//Create Queues
	Array<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos(4-1/*Graphics == Present*/);
	{
		VE_ASSERT(GraphicsQueueFamily.Index == PresentQueueFamily.Index);

		GraphicsQueueFamily.Queues.resize(GraphicsQueueFamily.QueuePriorities.size());
		PresentQueueFamily.Queues.resize(PresentQueueFamily.QueuePriorities.size());
		TransferQueueFamily.Queues.resize(TransferQueueFamily.QueuePriorities.size());
		ComputeQueueFamily.Queues.resize(ComputeQueueFamily.QueuePriorities.size());

		auto& GraphicsAndPresentQueueCreateInfo = DeviceQueueCreateInfos[0];
		GraphicsAndPresentQueueCreateInfo = VkDeviceQueueCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = GraphicsQueueFamily.Index,
			.queueCount = UInt32(GraphicsQueueFamily.Queues.size()),
			.pQueuePriorities = GraphicsQueueFamily.QueuePriorities.data()
		};

		auto& TransferQueueCreateInfo = DeviceQueueCreateInfos[1];
		TransferQueueCreateInfo = VkDeviceQueueCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex =TransferQueueFamily.Index,
			.queueCount = UInt32(TransferQueueFamily.Queues.size()),
			.pQueuePriorities = TransferQueueFamily.QueuePriorities.data()
		};

		auto& ComputeQueueCreateInfo = DeviceQueueCreateInfos[2];
		ComputeQueueCreateInfo = VkDeviceQueueCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = ComputeQueueFamily.Index,
			.queueCount = UInt32(ComputeQueueFamily.Queues.size()),
			.pQueuePriorities = ComputeQueueFamily.QueuePriorities.data()
		};
	}

	//Create Device
	VkDeviceCreateInfo DeviceCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			
		.pNext = nullptr/*m_physical_device_features2 .has_value()? &(m_physical_device_features2.value()) : */,
		.queueCreateInfoCount	= UInt32(DeviceQueueCreateInfos.size()),
		.pQueueCreateInfos		= DeviceQueueCreateInfos.data(),
		.enabledExtensionCount	= UInt32(Extensions.size()),
		.ppEnabledExtensionNames= Extensions.data(),
		.pEnabledFeatures = &GVulkan->GPU->GetFeatures()/*m_physical_device_features2.has_value() ? nullptr : &m_physical_device_features*/// (If pNext includes a VkPhysicalDeviceFeatures2, here should be NULL)
	};
	if(VK_SUCCESS != vkCreateDevice(
		GVulkan->GPU->GetHandle(),
		&DeviceCreateInfo,
		GVulkan->AllocationCallbacks,&Handle))
	{ throw SRuntimeError("Failed to create Vulkan Device!"); }

	//Retrieve Queues
	{
		for (UInt32 Idx = 0; Idx < GraphicsQueueFamily.Queues.size(); ++Idx)
		{ vkGetDeviceQueue(Handle, GraphicsQueueFamily.Index, Idx, &GraphicsQueueFamily.Queues[Idx]); }	

		PresentQueueFamily.Queues.front() = GraphicsQueueFamily.Queues.front();

		for (UInt32 Idx = 0; Idx < TransferQueueFamily.Queues.size(); ++Idx)
		{ vkGetDeviceQueue(Handle, TransferQueueFamily.Index, Idx, &TransferQueueFamily.Queues[Idx]); }	

		for (UInt32 Idx = 0; Idx < ComputeQueueFamily.Queues.size(); ++Idx)
		{ vkGetDeviceQueue(Handle, ComputeQueueFamily.Index, Idx, &ComputeQueueFamily.Queues[Idx]); }	
	}

	return Handle;
}

void FVulkanDevice::
Destroy()
{
	vkDestroyDevice(Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}
} } // namespace VE::Runtime