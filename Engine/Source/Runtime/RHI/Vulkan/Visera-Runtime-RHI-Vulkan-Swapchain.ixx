module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Swapchain;
import :Context;
import :Common;
import :GPU;
import :Device;
import :Surface;
import :Synchronization;

import Visera.Core.Signal;
import Visera.Runtime.Window;

export namespace VE
{

	class FVulkanSwapchain
	{
		friend class FVulkan;
	public:
		class SRecreation : public std::exception { public: SRecreation() noexcept = default; };
		void WaitForNextImage(const FVulkanSemaphore& _SignalSemaphore_);
		void Present(FVulkanSemaphore _WaitSemaphores[], UInt32 _WaitSemaphoreCount);

		auto GetCursor()		const   -> UInt32					{ return Cursor; }
		auto GetFrameCount()	const	-> UInt8					{ return Images.size(); }
		auto GetCurrentImage()	const   -> const VkImage			{ return Images[Cursor]; }
		auto GetExtent()		const	-> const VkExtent2D&		{ return ImageExtent; }
		auto GetFormat()		const	-> EVulkanFormat			{ return ImageFormat; }
		auto GetColorSpace()	const	-> VkColorSpaceKHR			{ return ImageColorSpace; }
		auto GetImages()		const	-> const Array<VkImage>&	{ return Images; }
		auto GetImageViews()	const	-> const Array<VkImageView> { return ImageViews; }
		auto GetHandle()		const	-> const VkSwapchainKHR		{ return Handle; }

	private:
		VkSwapchainKHR			Handle{ VK_NULL_HANDLE };
		
		EVulkanPresentMode		PresentMode		= EVulkanPresentMode::Mailbox;

		UInt32					Cursor{ 0 };	// Current Frame Index
		void MoveCursor(UInt32 Stride) { Cursor = (Cursor + Stride) % Images.size(); }

		Array<VkImage>			Images;			// Size: Clamp(minImageCount + 1, maxImageCount)
		Array<VkImageView>		ImageViews;
		VkExtent2D				ImageExtent;
		EVulkanFormat			ImageFormat		= EVulkanFormat::U32_sRGB_B8_G8_R8_A8;
		VkColorSpaceKHR			ImageColorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		EVulkanFormat			DepthImageFormat= EVulkanFormat::S32_Float_Depth32;
		EVulkanImageTiling		DepthImageTiling= EVulkanImageTiling::Optimal;

		void Create();
		void Destroy();

		FVulkanSwapchain() noexcept  = default;
		~FVulkanSwapchain() noexcept = default;
	};

	void FVulkanSwapchain::
	Create()
	{
		//First Time Creation
		if (Handle == VK_NULL_HANDLE)
		{
			Bool bImageFormatSupport = False;
			
			//Check Image Format Support
			for(const auto& SurfaceFormat : GVulkan->Surface->GetFormats())
			{
				if (SurfaceFormat.format	 != AutoCast(ImageFormat) ||
					SurfaceFormat.colorSpace != ImageColorSpace)
					continue;
				bImageFormatSupport = True;
			}
			if (!bImageFormatSupport) throw SRuntimeError("Failed to create the Swapchain since required Image FormatView is unsupported!");

			Bool bPresentModeSupport = False;
			//Check Image Format Support
			for(const auto& SurfacePresentMode : GVulkan->Surface->GetPresentModes())
			{
				if (SurfacePresentMode != AutoCast(PresentMode)) continue;
				bPresentModeSupport = True;
			}
			if (!bPresentModeSupport) throw SRuntimeError("Failed to create the Swapchain since required Present Mode is unsupported!");
		
			Bool bZBufferFormatSupport = False;
			//Check Depth Buffer (ZBuffer) Format
			//- 1. Tiling Linear
			auto ZBufferProperties = GVulkan->GPU->QueryFormatProperties(DepthImageFormat);
			switch (DepthImageTiling)
			{
			case EVulkanImageTiling::Linear:
				bZBufferFormatSupport = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & ZBufferProperties.linearTilingFeatures;
				break;
			case EVulkanImageTiling::Optimal:
				bZBufferFormatSupport = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & ZBufferProperties.optimalTilingFeatures;
				break;
			default: break;
			}
			if(!bZBufferFormatSupport) throw SRuntimeError("Failed to create the Swapchain since required ZBuffer FormatView is unsupported!");
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
		else throw SRuntimeError("Failed to create the Swapchain since the Surface Image Count is unsupported!");

		if (SurfaceCapabilities.currentExtent.height == UINT32_MAX)
		{
			auto FrameBufferExtent = Window::QueryFrameBufferExtent();
			while(!FrameBufferExtent.IsValid())
			{
				Window::PollEvents();
				Window::QueryFrameBufferExtent(&FrameBufferExtent);
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
					Window::PollEvents();
					GVulkan->Surface->GetCapabilities();
				}
				ImageExtent = SurfaceCapabilities.currentExtent;
		}

		Array<UInt32> QueuefamilyIndices
		{
			GVulkan->Device->GetQueueFamily(EVulkanQueueFamily::Graphics).Index,
			GVulkan->Device->GetQueueFamily(EVulkanQueueFamily::Present).Index
		};
		VkSwapchainCreateInfoKHR CreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface				= GVulkan->Surface->GetHandle(),
			.minImageCount			= UInt32(Images.size()), // Note that, this is just a minimum number of images in the swap chain, the implementation could make it more.
			.imageFormat			= AutoCast(ImageFormat),
			.imageColorSpace		= ImageColorSpace,
			.imageExtent			= ImageExtent,
			.imageArrayLayers		= 1,
			.imageUsage				= AutoCast( EVulkanImageUsage::ColorAttachment |
												EVulkanImageUsage::TransferDestination),
			.imageSharingMode		= GVulkan->GPU->IsDiscreteGPU()? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= GVulkan->GPU->IsDiscreteGPU()? 0U : 2U,
			.pQueueFamilyIndices	= GVulkan->GPU->IsDiscreteGPU()? nullptr : QueuefamilyIndices.data(),
			.preTransform			= SurfaceCapabilities.currentTransform, // Do not want any pretransformation
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= AutoCast(PresentMode),
			.clipped				= VK_TRUE, // Means that we do not care about the color of pixels that are obscured for the best performance. (P89)
			.oldSwapchain			= VK_NULL_HANDLE //[TODO] Add Old Swapchain
		};
		if(vkCreateSwapchainKHR(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Swapchain!"); }

		//Retrieve Swap Chain Images
		vkGetSwapchainImagesKHR(GVulkan->Device->GetHandle(), Handle, &RequiredImageCount, Images.data());

		// Create Image Views
		for (size_t Idx = 0; Idx < ImageViews.size(); ++Idx)
		{
			VkImageViewCreateInfo CreateInfo
			{
				.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image		= Images[Idx],
				.viewType	= AutoCast(EVulkanImageViewType::Image2D),
				.format		= AutoCast(ImageFormat),
				.components{
							.r = AutoCast(EVulkanSwizzle::Identity),
							.g = AutoCast(EVulkanSwizzle::Identity),
							.b = AutoCast(EVulkanSwizzle::Identity),
							.a = AutoCast(EVulkanSwizzle::Identity)
							},

				.subresourceRange{
							.aspectMask		= AutoCast(EVulkanImageAspect::Color),
							.baseMipLevel	= 0,
							.levelCount		= 1,
							.baseArrayLayer = 0,
							.layerCount		= 1
							}
			};
			if(vkCreateImageView(
				GVulkan->Device->GetHandle(),
				&CreateInfo,
				GVulkan->AllocationCallbacks,
				&ImageViews[Idx]) != VK_SUCCESS)
			{ throw SRuntimeError("Failed to create Vulkan Image View at the Swapchain!"); }
		}

		//Init Frames
		Cursor = 0;
	}

	void FVulkanSwapchain::
	Destroy()
	{
		for (auto ImageView : ImageViews)
		{
			vkDestroyImageView(GVulkan->Device->GetHandle(), ImageView, GVulkan->AllocationCallbacks);
		}

		vkDestroySwapchainKHR(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	void FVulkanSwapchain::
	WaitForNextImage(const FVulkanSemaphore& _SignalSemaphore)
	{
		auto Result = vkAcquireNextImageKHR(GVulkan->Device->GetHandle(),
											Handle,
											UINT64_MAX,
								/*Signal*/  _SignalSemaphore,
								/*Signal*/  VK_NULL_HANDLE,
											&Cursor);

		if (VK_ERROR_OUT_OF_DATE_KHR == Result ||
			VK_SUBOPTIMAL_KHR		 == Result)
		{
			//recreate_swapchain();
			throw SRecreation{};
		}
		if (Result != VK_SUCCESS)
		{ throw SRuntimeError("Failed to retrive the next image from the Vulkan Swapchain!"); }
	}

	void FVulkanSwapchain::
	Present(FVulkanSemaphore _WaitSemaphores[], UInt32 _WaitSemaphoreCount)
	{
		VkPresentInfoKHR PresentInfo
		{
			.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores	= reinterpret_cast<VkSemaphore*>(_WaitSemaphores),
			.swapchainCount		= _WaitSemaphoreCount,
			.pSwapchains		= &Handle,
			.pImageIndices		= &Cursor,
			.pResults			= nullptr // It is not necessary if you are only using a single swap chain
		};
		auto PresentQueue = GVulkan->Device->GetQueueFamily(EVulkanQueueFamily::Present).Queues[0];
		auto Result = vkQueuePresentKHR(PresentQueue, &PresentInfo);

		if (VK_ERROR_OUT_OF_DATE_KHR == Result ||
			VK_SUBOPTIMAL_KHR		 == Result)
		{
			//recreate_swapchain();
			throw SRecreation{};
		}
		if (Result != VK_SUCCESS)
		{ throw SRuntimeError(Text("Failed to present the Vulkan Swapchain! (Cursor:{})", Cursor)); }

		MoveCursor(1);
	}

} // namespace VE