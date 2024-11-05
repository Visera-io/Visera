module;
#include <ViseraEngine>

#include <volk.h>
export module Visera.Runtime.Render.RHI.Vulkan:Device;

import Visera.Core.Log;
import :Allocator;
import :Instance;
import :GPU;
import :Surface;

export namespace VE { namespace RHI
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanDevice
	{
		friend class VulkanContext;
	private:
		VkDevice            Handle{ VK_NULL_HANDLE };
		VulkanGPU			Host{};
		Array<RawString>	Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME,
									    VK_KHR_MAINTENANCE1_EXTENSION_NAME };

		struct QueueFamily
		{
			UInt32		   Index = UINT32_MAX;
			Array<VkQueue> Queues;
			Array<Float>   QueuePriorities;
			Bool IsValid() const { return Index != UINT32_MAX; }
		};
		struct
		{	//Required Queue Families and Their Queues
			QueueFamily	Graphics { .QueuePriorities{1.0} };
			QueueFamily	Present	 { .QueuePriorities{1.0} };
			QueueFamily	Transfer { .QueuePriorities{1.0} };
			QueueFamily	Compute	 { .QueuePriorities{1.0} };
		}QueueFamilies;
		operator VkDevice() const { return Handle; }

		void Create(const VulkanInstance& Instance, VulkanSurface* Surface);
		void Destroy(const VulkanInstance& Instance);

	public:
		VulkanDevice() noexcept = default;
		~VulkanDevice() noexcept = default;
	};

	void VulkanDevice::
	Create(const VulkanInstance& Instance, VulkanSurface* Surface)
	{
		//Find Suitable Host GPU
		auto GPUs = VulkanGPU::EnumerateAvailableGPUs(Instance);

		for (const auto& GPU : GPUs)
		{
			//Features
			{
				if (!GPU.IsDiscreteGPU()) continue;
				if (!GPU.GetFeatures().samplerAnisotropy) continue;
			}

			//Queue Families Properties
			{
				const auto& Properties = GPU.GetQueueFamilyProperties();
				Set<UInt32> GraphicsQueueFamilies;
				Set<UInt32> PresentQueueFamilies;
				Array<UInt32> TransferQueueFamilies;
				Array<UInt32> ComputeQueueFamilies;
				for (UInt32 Index = 0; Index < Properties.size(); ++Index)
				{
					if(VK_QUEUE_GRAPHICS_BIT & Properties[Index].queueFlags)
					{ GraphicsQueueFamilies.emplace(Index); }

					VkBool32 bPresentSupported = VK_FALSE;
					vkGetPhysicalDeviceSurfaceSupportKHR(GPU, Index, Surface->GetHandle(), &bPresentSupported);
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
					QueueFamilies.Graphics.Index = IdxA;
					QueueFamilies.Present.Index = IdxA;
					for (UInt32 IdxB : TransferQueueFamilies)
					{
						if (IdxB == IdxA) continue;
						QueueFamilies.Transfer.Index = IdxB;
						for (UInt32 IdxC : ComputeQueueFamilies)
						{
							if (IdxC == IdxB) continue;
							QueueFamilies.Compute.Index = IdxC;
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
				std::transform(GPU.GetExtensionProperties().begin(), GPU.GetExtensionProperties().end(),
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
				vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, Surface->GetHandle(), &FormatCount, nullptr);
				if (!FormatCount) continue;
				Array<VkSurfaceFormatKHR> Formats(FormatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, Surface->GetHandle(), &FormatCount, Formats.data());

				UInt32 PresentModeCount = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, Surface->GetHandle(), &PresentModeCount, nullptr);
				if (!PresentModeCount) continue;
				Array<VkPresentModeKHR> PresentModes(PresentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, Surface->GetHandle(), &PresentModeCount, PresentModes.data());
				Surface->SetFormats(std::move(Formats));
				Surface->SetPresentModes(std::move(PresentModes));
			}
			
			//Found Suitable Host GPU
			{ Host = std::move(GPU); break; }
		}
		if (Host.GetHandle() == VK_NULL_HANDLE) { Log::Fatal("Failed to find a suitable Physical Device on current computer!"); }

		//Create Queues
		Array<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos(4-1/*Graphics == Present*/);
		{
			Assert(QueueFamilies.Graphics.Index == QueueFamilies.Present.Index);
			QueueFamilies.Graphics.Queues.resize(QueueFamilies.Graphics.QueuePriorities.size());
			QueueFamilies.Present.Queues.resize(QueueFamilies.Present.QueuePriorities.size());
			QueueFamilies.Transfer.Queues.resize(QueueFamilies.Transfer.QueuePriorities.size());
			QueueFamilies.Compute.Queues.resize(QueueFamilies.Compute.QueuePriorities.size());

			auto& GraphicsAndPresentQueueCreateInfo = DeviceQueueCreateInfos[0];
			GraphicsAndPresentQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies.Graphics.Index,
				.queueCount = UInt32(QueueFamilies.Graphics.Queues.size()),
				.pQueuePriorities = QueueFamilies.Graphics.QueuePriorities.data()
			};

			auto& TransferQueueCreateInfo = DeviceQueueCreateInfos[1];
			TransferQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies.Transfer.Index,
				.queueCount = UInt32(QueueFamilies.Transfer.Queues.size()),
				.pQueuePriorities = QueueFamilies.Transfer.QueuePriorities.data()
			};

			auto& ComputeQueueCreateInfo = DeviceQueueCreateInfos[2];
			ComputeQueueCreateInfo = VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = QueueFamilies.Compute.Index,
				.queueCount = UInt32(QueueFamilies.Compute.Queues.size()),
				.pQueuePriorities = QueueFamilies.Compute.QueuePriorities.data()
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
			.pEnabledFeatures = &Host.GetFeatures()/*m_physical_device_features2.has_value() ? nullptr : &m_physical_device_features*/// (If pNext includes a VkPhysicalDeviceFeatures2, here should be NULL)
		};
		VK_CHECK(vkCreateDevice(Host, &DeviceCreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));

		//Retrieve Queues
		{
			for (UInt32 Idx = 0; Idx < QueueFamilies.Graphics.Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies.Graphics.Index, Idx, &QueueFamilies.Graphics.Queues[Idx]); }	

			QueueFamilies.Present.Queues.front() = QueueFamilies.Graphics.Queues.front();

			for (UInt32 Idx = 0; Idx < QueueFamilies.Transfer.Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies.Transfer.Index, Idx, &QueueFamilies.Transfer.Queues[Idx]); }	

			for (UInt32 Idx = 0; Idx < QueueFamilies.Compute.Queues.size(); ++Idx)
			{ vkGetDeviceQueue(Handle, QueueFamilies.Compute.Index, Idx, &QueueFamilies.Compute.Queues[Idx]); }	
		}
	}

	void VulkanDevice::
	Destroy(const VulkanInstance& Instance)
	{
		vkDestroyDevice(Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::RHI