module;
#include <ViseraEngine>

#include <volk.h>
export module Visera.Render.RHI.Vulkan:Device;

import Visera.Core.Log;
import :Instance;
import :GPU;
import :Surface;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class VulkanContext;

	class VulkanDevice
	{
		friend class VulkanContext;
	private:
		VkDevice            Handle{ VK_NULL_HANDLE };
		VulkanGPU			Host{};
		Set<String>			Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME,
									    VK_KHR_MAINTENANCE1_EXTENSION_NAME };

		struct QueueFamily
		{
			struct Queue
			{
				VkQueue Handle{ VK_NULL_HANDLE };
				Float	Priority = 1.0f;
				operator VkQueue() const { return Handle; }
			};

			Array<Queue> Queues;
			UInt32		 Index = UINT32_MAX;
			Bool IsValid() const { return Index != UINT32_MAX; }
		};
		struct
		{	//Required Queue Families and Their Queues
			QueueFamily	Graphics	{.Queues{{.Priority = 1.0}}};
			QueueFamily	Present		{.Queues{{.Priority = 1.0}}};
			QueueFamily	Transfer	{.Queues{{.Priority = 1.0}}};
			QueueFamily	Compute		{.Queues{{.Priority = 1.0}}};
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
				const auto& AvailableExtensions = GPU.GetExtensionProperties();
				UInt32 FoundCount = 0;
				for (const auto& AvailableExtension : AvailableExtensions)
				{
					if (Extensions.count(AvailableExtension.extensionName)) FoundCount++;
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
	}

	void VulkanDevice::
	Destroy(const VulkanInstance& Instance)
	{
		
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE