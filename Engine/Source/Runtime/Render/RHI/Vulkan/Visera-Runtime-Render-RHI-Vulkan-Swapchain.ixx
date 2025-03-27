module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Swapchain;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :GPU;
import :Device;
import :Surface;
import :Synchronization;

import Visera.Core.Signal;
import Visera.Core.Log;
import Visera.Runtime.Platform.Window;

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
		auto GetColorSpace()	const	-> EVulkanColorSpace		{ return ImageColorSpace; }
		auto GetImages()		const	-> const Array<VkImage>&	{ return Images; }
		auto GetHandle()		const	-> const VkSwapchainKHR		{ return Handle; }

	private:
		VkSwapchainKHR			Handle{ VK_NULL_HANDLE };
		
		EVulkanPresentMode		PresentMode		= EVulkanPresentMode::Mailbox; // If Mailbox mode is NOT supported, use FIFO by default.

		UInt32					Cursor{ 0 };	// Current Frame Index
		void MoveCursor(UInt32 Stride) { Cursor = (Cursor + Stride) % Images.size(); }

		Array<VkImage>			Images;			// Size: Clamp(minImageCount + 1, maxImageCount)
		VkExtent2D				ImageExtent;
		EVulkanFormat			ImageFormat		= EVulkanFormat::U32_sRGB_B8_G8_R8_A8;
		EVulkanColorSpace		ImageColorSpace	= EVulkanColorSpace::sRGB_Nonlinear;

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
			//Check Image Format Support
			Bool bImageFormatSupport = False;
			for(const auto& SurfaceFormat : GVulkan->Surface->GetFormats())
			{
				if (SurfaceFormat.format	 != AutoCast(ImageFormat) ||
					SurfaceFormat.colorSpace != AutoCast(ImageColorSpace))
					continue;
				bImageFormatSupport = True;
			}
			if (!bImageFormatSupport)
			{ throw SRuntimeError("Failed to create the Swapchain since required Image FormatView is unsupported!"); }

			Bool bPresentModeSupport = False;
			//Check Image Format Support
			for(const auto& SurfacePresentMode : GVulkan->Surface->GetPresentModes())
			{
				if (SurfacePresentMode != AutoCast(PresentMode)) continue;
				bPresentModeSupport = True;
			}
			if (!bPresentModeSupport)
			{
				Log::Warn("Preferred Swapchain Present Mode is NOT supported! - Using FIFO mode by default.");
				PresentMode = EVulkanPresentMode::FIFO;
			}
		
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

		Segment<UInt32, 2> QueuefamilyIndices
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
			.imageColorSpace		= AutoCast(ImageColorSpace),
			.imageExtent			= ImageExtent,
			.imageArrayLayers		= 1,
			.imageUsage				= AutoCast( EVulkanImageUsage::ColorAttachment |
												EVulkanImageUsage::TransferDestination),
			.imageSharingMode		= GVulkan->Device->IsSupportingExclusiveSwapchain()? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= GVulkan->Device->IsSupportingExclusiveSwapchain()? 0U		 : UInt32(QueuefamilyIndices.size()),
			.pQueueFamilyIndices	= GVulkan->Device->IsSupportingExclusiveSwapchain()? nullptr : QueuefamilyIndices.data(),
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

		//Init Frames
		Cursor = 0;
	}

	void FVulkanSwapchain::
	Destroy()
	{
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