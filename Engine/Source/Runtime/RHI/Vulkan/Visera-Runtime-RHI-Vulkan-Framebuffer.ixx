module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Framebuffer;

import :Allocator;
import :Common;
import :Device;
import :Swapchain;
import :RenderPassLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanFramebuffer
	{
		friend class FVulkanRenderPass;
	public:
		Bool HasDepthStencil()		const { return DepthStencilImage != nullptr; }
	
		auto GetRenderArea()		const -> VkRect2D						{ return RenderArea; }
		auto GetDepthStencil()		const -> SharedPtr<const FVulkanImage>	{ return DepthStencilImage; }

		auto GetHandle()				  -> VkFramebuffer					{ return Handle; }

	private:
		VkFramebuffer						 Handle{ VK_NULL_HANDLE };
		const VkRenderPass const			 Owner { VK_NULL_HANDLE };
		VkRect2D							 RenderArea{};

		Array<SharedPtr<FVulkanImage>>		 ColorImages;
		Array<SharedPtr<FVulkanImageView>>	 ColorImageViews;
		Array<SharedPtr<FVulkanImage>>		 ResolveColorImages; // MSAA Result
		Array<SharedPtr<FVulkanImageView>>	 ResolveColorImageViews;
		SharedPtr<FVulkanImage>				 DepthStencilImage;
		SharedPtr<FVulkanImageView>			 DepthStencilImageView;
		//[TODO]: Shading Rate Image
		Array<VkClearValue>					 ClearColors;

		VkAttachmentDescriptionStencilLayout StencilDescription;

		void Create(const FVulkanRenderPassLayout& _OwnerLayout);
		void Destroy();
		
	public:
		FVulkanFramebuffer(const VkRenderPass const _Owner);
		~FVulkanFramebuffer();
	};

	FVulkanFramebuffer::
	FVulkanFramebuffer(const VkRenderPass const _Owner)
		: Owner{ _Owner }
	{
		VE_ASSERT(Owner != VK_NULL_HANDLE);
	}

	FVulkanFramebuffer::
	~FVulkanFramebuffer()
	{	
		Destroy();
	}

	void FVulkanFramebuffer::
	Create(const FVulkanRenderPassLayout& _OwnerLayout)
	{
		RenderArea.offset = _OwnerLayout.GetRenderAreaOffset2D();
		RenderArea.extent = _OwnerLayout.GetRenderAreaExtent2D();

		VE_ASSERT(_OwnerLayout.ColorImageCount && _OwnerLayout.ColorImageCount % 2 == 0);

		ColorImages.resize(_OwnerLayout.ColorImageCount);
		ColorImageViews.resize(ColorImages.size());
		ResolveColorImages.resize(ColorImages.size());
		ResolveColorImageViews.resize(ResolveColorImages.size());

		for (UInt32 ColorImageIdx = 0; ColorImageIdx < _OwnerLayout.ColorImageCount; ColorImageIdx++)
		{
			// Color Images
			auto& ColorAttachmentDescription = _OwnerLayout.AttachmentDescriptions[ColorImageIdx << 1];
			VE_ASSERT(ColorAttachmentDescription.Image != nullptr);

			ColorImages[ColorImageIdx] = ColorAttachmentDescription.Image;
			ColorImageViews[ColorImageIdx] = ColorImages[ColorImageIdx]->CreateImageView(ColorAttachmentDescription.ImageViewType);
			
			// Resolve Color Images
			auto& ResolveColorAttachmentDescription = _OwnerLayout.AttachmentDescriptions[(ColorImageIdx << 1) + 1];
			VE_ASSERT(ResolveColorAttachmentDescription.Image != nullptr);

			ResolveColorImages[ColorImageIdx] = ResolveColorAttachmentDescription.Image;
			ResolveColorImageViews[ColorImageIdx] = ResolveColorImages[ColorImageIdx]->CreateImageView(ResolveColorAttachmentDescription.ImageViewType);
		}

		if (_OwnerLayout.HasDepthImage())
		{
			auto& DepthImageDescription = _OwnerLayout.AttachmentDescriptions[_OwnerLayout.ColorImageCount];
			DepthStencilImage = DepthImageDescription.Image;
			VE_ASSERT(DepthStencilImage != nullptr);
			DepthStencilImageView = DepthStencilImage->CreateImageView(DepthImageDescription.ImageViewType);
		}

		if (_OwnerLayout.StencilDescription.has_value())
		{
			VE_WIP;
			/*DepthStencilImage = GVulkan->Allocator->CreateImage(EImageType::Image2D,
																_OwnerLayout->GetExtent(),
																EImageAspect::Depth | EImageAspect::Stencil,
																EImageUsage::DepthStencilAttachment);*/
		}

		Array<VkImageView> AttachmentViews(ColorImages.size() * 2 + HasDepthStencil());
		for (UInt8 Idx = 0; Idx < ColorImages.size(); Idx++)
		{
			AttachmentViews[Idx * 2]	 = ColorImageViews[Idx]->GetHandle();
			AttachmentViews[Idx * 2 + 1] = ResolveColorImageViews[Idx]->GetHandle();
		}
		if (HasDepthStencil) { AttachmentViews.back() = DepthStencilImageView->GetHandle(); }

		VkFramebufferCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.renderPass = Owner,
			.attachmentCount = UInt32(AttachmentViews.size()),
			.pAttachments	 = AttachmentViews.data(),
			.width  = _OwnerLayout.GetRenderAreaExtent3D().width,
			.height = _OwnerLayout.GetRenderAreaExtent3D().height,
			.layers = _OwnerLayout.GetRenderAreaExtent3D().depth,
		};

		if (vkCreateFramebuffer(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Framebuffer!"); }
	}

	void FVulkanFramebuffer::
	Destroy()
	{
		ColorImageViews.clear();
		ColorImages.clear();
		DepthStencilImageView.reset();
		DepthStencilImage.reset();

		vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime