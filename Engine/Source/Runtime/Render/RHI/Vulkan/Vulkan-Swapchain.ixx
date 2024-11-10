module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Swapchain;

import :Context;
import :Allocator;
import :GPU;
import :Device;
import :Surface;
import :Synchronization;

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
		auto GetCursor()			const   -> UInt32					{ return Cursor; }
		auto GetExtent()			const	-> const VkExtent2D&		{ return ImageExtent; }
		auto GetFormat()			const	-> VkFormat					{ return ImageFormat; }
		auto GetColorSpace()		const	-> VkColorSpaceKHR			{ return ImageColorSpace; }
		auto GetImages()			const	-> const Array<VkImage>&	{ return Images; }
		auto GetCurrentImage()		const	-> VkImage					{ return Images[Cursor]; }
		auto GetHandle()			const	-> VkSwapchainKHR			{ return Handle; }
		operator VkSwapchainKHR()	const	{ return Handle; }

	private:
		VkSwapchainKHR			Handle{ VK_NULL_HANDLE };
				
		VkPresentModeKHR		PresentMode		= VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		UInt32					Cursor{ 0 };	// Current image index for rendering

		Array<VkImage>			Images;			// Size: Clamp(minImageCount + 1, maxImageCount)
		Array<VkImageView>		ImageViews;
		VkExtent2D				ImageExtent;
		VkFormat				ImageFormat		= VK_FORMAT_B8G8R8A8_SRGB;
		VkColorSpaceKHR			ImageColorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		VkFormat				ZBufferFormat	= VK_FORMAT_D32_SFLOAT;
		VkImageTiling			ZBufferTiling	= VK_IMAGE_TILING_OPTIMAL;

		void Create();
		void Destroy();

		VulkanSwapchain() noexcept  = default;
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
			for(const auto& SurfaceFormat : GVulkan->Surface->GetFormats())
			{
				if (SurfaceFormat.format	 != ImageFormat ||
					SurfaceFormat.colorSpace != ImageColorSpace)
					continue;
				bImageFormatSupport = True;
			}
			if (!bImageFormatSupport) Log::Fatal("Failed to create the Swapchain since required Image Format is unsupported!");

			Bool bPresentModeSupport = False;
			//Check Image Format Support
			for(const auto& SurfacePresentMode : GVulkan->Surface->GetPresentModes())
			{
				if (SurfacePresentMode != PresentMode) continue;
				bPresentModeSupport = True;
			}
			if (!bPresentModeSupport) Log::Fatal("Failed to create the Swapchain since required Present Mode is unsupported!");
		
			Bool bZBufferFormatSupport = False;
			//Check Depth Buffer (ZBuffer) Format
			//- 1. Tiling Linear
			auto ZBufferProperties = GVulkan->GPU->QueryFormatProperties(ZBufferFormat);
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
		
		auto& SurfaceCapabilities = GVulkan->Surface->GetCapabilities();
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
					GVulkan->Surface->GetCapabilities();
				}
				ImageExtent = SurfaceCapabilities.currentExtent;
		}

		Array<UInt32> QueuefamilyIndices
		{
			GVulkan->Device->GetQueueFamily(GVulkan->Device->Graphics).Index,
			GVulkan->Device->GetQueueFamily(GVulkan->Device->Present).Index
		};
		VkSwapchainCreateInfoKHR CreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface				= GVulkan->Surface->GetHandle(),
			.minImageCount			= UInt32(Images.size()), // Note that, this is just a minimum number of images in the swap chain, the implementation could make it more.
			.imageFormat			= ImageFormat,
			.imageColorSpace		= ImageColorSpace,
			.imageExtent			= ImageExtent,
			.imageArrayLayers		= 1,
			.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
									  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode		= GVulkan->GPU->IsDiscreteGPU()? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= GVulkan->GPU->IsDiscreteGPU()? 0U : 2U,
			.pQueueFamilyIndices	= GVulkan->GPU->IsDiscreteGPU()? nullptr : QueuefamilyIndices.data(),
			.preTransform			= SurfaceCapabilities.currentTransform, // Do not want any pretransformation
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= PresentMode,
			.clipped				= VK_TRUE, // Means that we do not care about the color of pixels that are obscured for the best performance. (P89)
			.oldSwapchain			= VK_NULL_HANDLE //[TODO] Add Old Swapchain
		};
		VK_CHECK(vkCreateSwapchainKHR(GVulkan->Device->GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));

		//Retrieve Swap Chain Images
		vkGetSwapchainImagesKHR(GVulkan->Device->GetHandle(), Handle, &RequiredImageCount, Images.data());

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
			VK_CHECK(vkCreateImageView(GVulkan->Device->GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks,&ImageViews[Idx]));
		}

		//Reset Cursor
		Cursor = 0;
	}

	void VulkanSwapchain::
	Destroy()
	{
		for (auto ImageView : ImageViews)
		{
			vkDestroyImageView(GVulkan->Device->GetHandle(), ImageView, VulkanAllocator::AllocationCallbacks);
		}
		vkDestroySwapchainKHR(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime