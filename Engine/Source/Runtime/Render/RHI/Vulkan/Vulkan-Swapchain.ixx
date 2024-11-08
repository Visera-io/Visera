module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Swapchain;

import :Allocator;
import :GPU;
import :Device;
import :Surface;

import Visera.Engine.Core.Log;
import Visera.Engine.Runtime.Platform;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanSwapchain
	{
		friend class Vulkan;
	public:
		auto GetHandle()			const	-> VkSwapchainKHR { return Handle; }
		operator VkSwapchainKHR()	const { return Handle; }

	private:
		VkSwapchainKHR			Handle{ VK_NULL_HANDLE };
		const VulkanDevice&		HostDevice;
		const VulkanSurface&	HostSurface;
				
		VkPresentModeKHR		PresentMode		= VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		uint32_t				Cursor{ 0 };	// Current image index for rendering

		Array<VkImage>			Images;			// Size: Clamp(minImageCount + 1, maxImageCount)
		Array<VkImageView>		ImageViews;
		VkExtent2D				ImageExtent;
		VkFormat				ImageFormat		= VK_FORMAT_B8G8R8A8_SRGB;
		VkColorSpaceKHR			ImageColorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		VkFormat				ZBufferFormat	= VK_FORMAT_D32_SFLOAT;
		VkImageTiling			ZBufferTiling	= VK_IMAGE_TILING_OPTIMAL;

		void Create();
		void Destroy();

	public:
		VulkanSwapchain(const VulkanDevice& Device, const VulkanSurface& Suface) noexcept : HostDevice{ Device }, HostSurface{ Suface } {};
		~VulkanSwapchain() noexcept = default;
	};

	void VulkanSwapchain::
	Create()
	{
		//First Time Creation
		if (Handle == VK_NULL_HANDLE)
		{
			Bool bImageFormatSupport = False;
			//Check Image Format Support
			for(const auto& SurfaceFormat : HostSurface.GetFormats())
			{
				if (SurfaceFormat.format	 != ImageFormat ||
					SurfaceFormat.colorSpace != ImageColorSpace)
					continue;
				bImageFormatSupport = True;
			}
			if (!bImageFormatSupport) Log::Fatal("Failed to create the Swapchain since required Image Format is unsupported!");

			Bool bPresentModeSupport = False;
			//Check Image Format Support
			for(const auto& SurfacePresentMode : HostSurface.GetPresentModes())
			{
				if (SurfacePresentMode != PresentMode) continue;
				bPresentModeSupport = True;
			}
			if (!bPresentModeSupport) Log::Fatal("Failed to create the Swapchain since required Present Mode is unsupported!");
		
			Bool bZBufferFormatSupport = False;
			//Check Depth Buffer (ZBuffer) Format
			//- 1. Tiling Linear
			auto ZBufferProperties = HostDevice.QueryFormatProperties(ZBufferFormat);
			switch (ZBufferTiling)
			{
			case VK_IMAGE_TILING_LINEAR:
				bZBufferFormatSupport = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & ZBufferProperties.linearTilingFeatures;
				break;
			case VK_IMAGE_TILING_OPTIMAL:
				bZBufferFormatSupport = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & ZBufferProperties.optimalTilingFeatures;
				break;
			default: break;
			}
			if(!bZBufferFormatSupport) Log::Fatal("Failed to create the Swapchain since required ZBuffer Format is unsupported!");
		}
		
		auto SurfaceCapabilities = HostDevice.QuerySurfaceCapabilities(HostSurface);
		UInt32 RequiredImageCount = std::clamp(
									SurfaceCapabilities.minImageCount + 1,
									SurfaceCapabilities.minImageCount + 1,
									SurfaceCapabilities.maxImageCount);
		if (RequiredImageCount)
		{
			Images.resize(RequiredImageCount);
			ImageViews.resize(RequiredImageCount);
		}
		else Log::Fatal("Failed to create the Swapchain since the Surface Image Count is unsupported!");

		if (SurfaceCapabilities.currentExtent.height == UINT32_MAX)
		{
			auto FrameBufferExtent = Platform::GetWindow().QueryFrameBufferExtent();
			while(!FrameBufferExtent.IsValid())
			{
				Platform::GetWindow().PollEvents();
				Platform::GetWindow().QueryFrameBufferExtent(&FrameBufferExtent);
			}
			ImageExtent = VkExtent2D
			{
				.width	= std::clamp(UInt32(FrameBufferExtent.Width),
									 SurfaceCapabilities.minImageExtent.width,
									 SurfaceCapabilities.maxImageExtent.width),
				.height = std::clamp(UInt32(FrameBufferExtent.Height),
									 SurfaceCapabilities.minImageExtent.height,
									 SurfaceCapabilities.maxImageExtent.height)
			};
		}
		else
		{
			while (SurfaceCapabilities.currentExtent.width <= 0 ||
				   SurfaceCapabilities.currentExtent.height <= 0)
				{
					Platform::GetWindow().PollEvents();
					SurfaceCapabilities = HostDevice.QuerySurfaceCapabilities(HostSurface);
				}
				ImageExtent = SurfaceCapabilities.currentExtent;
		}

		Array<UInt32> QueuefamilyIndices
		{
			HostDevice.GetQueueFamily(HostDevice.Graphics).Index,
			HostDevice.GetQueueFamily(HostDevice.Present).Index
		};
		VkSwapchainCreateInfoKHR CreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface				= HostSurface.GetHandle(),
			.minImageCount			= UInt32(Images.size()), // Note that, this is just a minimum number of images in the swap chain, the implementation could make it more.
			.imageFormat			= ImageFormat,
			.imageColorSpace		= ImageColorSpace,
			.imageExtent			= ImageExtent,
			.imageArrayLayers		= 1,
			.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
									  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode		= HostDevice.IsDiscreteGPU()? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= HostDevice.IsDiscreteGPU()? 0U : 2U,
			.pQueueFamilyIndices	= HostDevice.IsDiscreteGPU()? nullptr : QueuefamilyIndices.data(),
			.preTransform			= SurfaceCapabilities.currentTransform, // Do not want any pretransformation
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= PresentMode,
			.clipped				= VK_TRUE, // Means that we do not care about the color of pixels that are obscured for the best performance. (P89)
			.oldSwapchain			= VK_NULL_HANDLE //[TODO] Add Old Swapchain
		};
		VK_CHECK(vkCreateSwapchainKHR(HostDevice.GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));

		//Retrieve Swap Chain Images
		vkGetSwapchainImagesKHR(HostDevice.GetHandle(), Handle, &RequiredImageCount, Images.data());

		// Create Image Views
		for (size_t Idx = 0; Idx < ImageViews.size(); ++Idx)
		{
			VkImageViewCreateInfo CreateInfo
			{
				.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image		= Images[Idx],
				.viewType	= VK_IMAGE_VIEW_TYPE_2D,
				.format		= ImageFormat,
				.components{
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY
							},
				.subresourceRange{
							.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel	= 0,
							.levelCount		= 1,
							.baseArrayLayer = 0,
							.layerCount		= 1
							}
			};
			VK_CHECK(vkCreateImageView(HostDevice.GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks,&ImageViews[Idx]));
		}

		//Reset Cursor
		Cursor = 0;
	}

	void VulkanSwapchain::
	Destroy()
	{
		for (auto ImageView : ImageViews)
		{
			vkDestroyImageView(HostDevice.GetHandle(), ImageView, VulkanAllocator::AllocationCallbacks);
		}
		vkDestroySwapchainKHR(HostDevice.GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime