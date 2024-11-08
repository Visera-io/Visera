module;
#include <Visera>

#include <volk.h>
export module Visera.Engine.Runtime.Render.RHI.Vulkan:Device;

import Visera.Engine.Core.Log;
import :Context;
import :Instance;
import :Allocator;
import :GPU;
import :Surface;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanDevice
	{
		friend class Vulkan;
	public:
		auto GetHandle() const	-> VkDevice	{ return Handle; }
		operator VkDevice() const	{ return Handle; }

	private:
		VkDevice				Handle{ VK_NULL_HANDLE };
		Array<RawString>		Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME,
											VK_KHR_MAINTENANCE1_EXTENSION_NAME };
	public:
		enum QueueFamilyType {Graphics, Present, Transfer, Compute, MAX_QUEUE_FAMILY_TYPE};
		struct QueueFamily
		{
			UInt32				Index = UINT32_MAX;
			Array<VkQueue>		Queues;
			Array<Float>		QueuePriorities{ 1.0 };
			Bool IsValid() const { return Index != UINT32_MAX; }
		};
		auto GetQueueFamily(QueueFamilyType Type) const -> const QueueFamily& { return QueueFamilies[Type]; }

	private:
		Array<QueueFamily> QueueFamilies;

		void Create(VulkanGPU* GPU, VulkanSurface* Surface);
		void Destroy();

		VulkanDevice()	noexcept	= default;
		~VulkanDevice() noexcept	= default;
	};

	void VulkanDevice::
	Create(VulkanGPU* GPU, VulkanSurface* Surface)
	{
		//Find Suitable Host GPU
		auto GPUs = GVulkan->Instance->EnumerateAvailableGPUs();

		for (const auto& GPUCandidate : GPUs)
		{
			//Features
			{
				if (!GPUCandidate.IsDiscreteGPU()) continue;
				if (!GPUCandidate.GetFeatures().samplerAnisotropy) continue;
			}

			//Queue Families Properties
			QueueFamilies.resize(MAX_QUEUE_FAMILY_TYPE);
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
					vkGetPhysicalDeviceSurfaceSupportKHR(GPUCandidate, Index, Surface->GetHandle(), &bPresentSupported);
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
				{ Log::Fatal("Failed to find a queue family supporting both Graphics and Present!"); }

				Bool Found = False;
				for (UInt32 IdxA : GraphicsAndPresentQueueFamilies)
				{
					QueueFamilies[Graphics].Index = IdxA;
					QueueFamilies[Present].Index = IdxA;
					for (UInt32 IdxB : TransferQueueFamilies)
					{
						if (IdxB == IdxA) continue;
						QueueFamilies[Transfer].Index = IdxB;
						for (UInt32 IdxC : ComputeQueueFamilies)
						{
							if (IdxC == IdxB) continue;
							QueueFamilies[Compute].Index = IdxC;
							Found = True;
						}
						if (Found) break;
					}
					if (Found) break;
				}
				if (!Found) Log::Fatal("Failed to find queue families that fit all requirements!");
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
				vkGetPhysicalDeviceSurfaceFormatsKHR(GPUCandidate, Surface->GetHandle(), &FormatCount, nullptr);
				if (!FormatCount) continue;
				Array<VkSurfaceFormatKHR> Formats(FormatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(GPUCandidate, Surface->GetHandle(), &FormatCount, Formats.data());

				UInt32 PresentModeCount = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(GPUCandidate, Surface->GetHandle(), &PresentModeCount, nullptr);
				if (!PresentModeCount) continue;
				Array<VkPresentModeKHR> PresentModes(PresentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(GPUCandidate, Surface->GetHandle(), &PresentModeCount, PresentModes.data());
				Surface->SetFormats(std::move(Formats));
				Surface->SetPresentModes(std::move(PresentModes));
			}
			
			//Found Suitable Host GPU
			{ *GPU = std::move(GPUCandidate); break; }
		}
		if (GVulkan->GPU->GetHandle() == VK_NULL_HANDLE) { Log::Fatal("Failed to find a suitable Physical Device on current computer!"); }

		//Create Queues
		Array<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos(4-1/*Graphics == Present*/);
		{
			Assert(QueueFamilies[Graphics].Index == QueueFamilies[Present].Index);
			QueueFamilies[Graphics].Queues.resize(QueueFamilies[Graphics].QueuePriorities.size());
			QueueFamilies[Present].Queues.resize(QueueFamilies[Present].QueuePriorities.size());
			QueueFamilies[Transfer].Queues.resize(QueueFamilies[Transfer].QueuePriorities.size());
			QueueFamilies[Compute].Queues.resize(QueueFamilies[Compute].QueuePriorities.size());

			auto& GraphicsAndPresentQueueCreateInfo = DeviceQueueCreateInfos[0];
			GraphicsAndPresentQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies[Graphics].Index,
				.queueCount = UInt32(QueueFamilies[Graphics].Queues.size()),
				.pQueuePriorities = QueueFamilies[Graphics].QueuePriorities.data()
			};

			auto& TransferQueueCreateInfo = DeviceQueueCreateInfos[1];
			TransferQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies[Transfer].Index,
				.queueCount = UInt32(QueueFamilies[Transfer].Queues.size()),
				.pQueuePriorities = QueueFamilies[Transfer].QueuePriorities.data()
			};

			auto& ComputeQueueCreateInfo = DeviceQueueCreateInfos[2];
			ComputeQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies[Compute].Index,
				.queueCount = UInt32(QueueFamilies[Compute].Queues.size()),
				.pQueuePriorities = QueueFamilies[Compute].QueuePriorities.data()
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
		VK_CHECK(vkCreateDevice(GVulkan->GPU->GetHandle(), &DeviceCreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));

		//Retrieve Queues
		{
			for (UInt32 Idx = 0; Idx < QueueFamilies[Graphics].Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies[Graphics].Index, Idx, &QueueFamilies[Graphics].Queues[Idx]); }	

			QueueFamilies[Present].Queues.front() = QueueFamilies[Graphics].Queues.front();

			for (UInt32 Idx = 0; Idx < QueueFamilies[Transfer].Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies[Transfer].Index, Idx, &QueueFamilies[Transfer].Queues[Idx]); }	

			for (UInt32 Idx = 0; Idx < QueueFamilies[Compute].Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies[Compute].Index, Idx, &QueueFamilies[Compute].Queues[Idx]); }	
		}
	}

	void VulkanDevice::
	Destroy()
	{
		vkDestroyDevice(Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime