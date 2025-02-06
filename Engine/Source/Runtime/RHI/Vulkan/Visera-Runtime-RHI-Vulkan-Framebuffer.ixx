module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Framebuffer;

import :Allocator;
import :Common;
import :Device;
import :Swapchain;
import :RenderPassLayout;
import :RenderPassResource;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanFramebuffer
	{
		friend class FVulkanRenderPass;
	public:
		Bool HasDepthStencil()		const { return DepthStencilImageView != nullptr; }
	
		auto GetRenderArea()		const -> VkRect2D				{ return RenderArea; }

		auto GetHandle()			const -> const VkFramebuffer	{ return Handle; }

	private:
		VkFramebuffer						 Handle{ VK_NULL_HANDLE };
		const VkRenderPass	const			 Owner { VK_NULL_HANDLE };
		VkRect2D							 RenderArea{};

		Array<SharedPtr<FVulkanImageView>>	 ColorImageViews;
		Array<SharedPtr<FVulkanImageView>>	 ResolveImageViews;
		SharedPtr<FVulkanImageView>			 DepthStencilImageView;
		//[TODO]: Shading Rate Image
		VkAttachmentDescriptionStencilLayout StencilDescription;

	public:
		FVulkanFramebuffer(const VkRenderPass const _Owner, SharedPtr<const FVulkanRenderPassLayout> _OwnerLayout, SharedPtr<const FVulkanRenderPassResource> _OwnerRenderTargets);
		~FVulkanFramebuffer();
	};

	FVulkanFramebuffer::
	FVulkanFramebuffer(
		const VkRenderPass const _Owner,
		SharedPtr<const FVulkanRenderPassLayout>   _OwnerLayout,
		SharedPtr<const FVulkanRenderPassResource> _OwnerRenderTargets)
		: Owner{ _Owner }
	{
		VE_ASSERT(_OwnerLayout->ColorImageCount && _OwnerLayout->ColorImageCount % 2 == 0);
		VE_ASSERT(_OwnerLayout->ColorImageCount <= _OwnerRenderTargets->ColorAttachments.size());
		VE_ASSERT(!_OwnerLayout->HasDepthImage() ||
				 ( _OwnerLayout->HasDepthImage() && _OwnerRenderTargets->HasDepthAttachment()));

		RenderArea.offset = _OwnerLayout->GetRenderAreaOffset2D();
		RenderArea.extent = _OwnerLayout->GetRenderAreaExtent2D();

		// Color Images & Resolve Color Images
		ColorImageViews.resize(_OwnerLayout->ColorImageCount);
		ResolveImageViews.resize(_OwnerLayout->ColorImageCount);
		
		for (UInt32 ColorImageIdx = 0; ColorImageIdx < _OwnerLayout->ColorImageCount; ColorImageIdx++)
		{
			auto& ColorAttachmentDescription = 
				_OwnerLayout->AttachmentDescriptions[ColorImageIdx];
			ColorImageViews[ColorImageIdx] = 
				_OwnerRenderTargets->ColorAttachments[ColorImageIdx].
				Image->CreateImageView(ColorAttachmentDescription.ImageViewType);
			
			auto& ResolveAttachmentDescription = 
				_OwnerLayout->AttachmentDescriptions[_OwnerLayout->ColorImageCount + ColorImageIdx];
			ResolveImageViews[ColorImageIdx] = 
				_OwnerRenderTargets->ColorAttachments[ColorImageIdx].
				ResolveImage->CreateImageView(ResolveAttachmentDescription.ImageViewType);
		}

		if (_OwnerLayout->HasDepthImage())
		{
			auto& DepthImageDescription = _OwnerLayout->AttachmentDescriptions[2 * _OwnerLayout->ColorImageCount + 1];
			DepthStencilImageView = _OwnerRenderTargets->DepthAttachment->CreateImageView(DepthImageDescription.ImageViewType);
		}

		if (_OwnerLayout->StencilDescription.has_value())
		{
			VE_WIP;
			/*DepthStencilImage = GVulkan->Allocator->CreateImage(EImageType::Image2D,
																_OwnerLayout->GetExtent(),
																EImageAspect::Depth | EImageAspect::Stencil,
																EImageUsage::DepthStencilAttachment);*/
		}

		Array<VkImageView> AttachmentViews;
		for (auto& ColorImageView   : ColorImageViews)	 { AttachmentViews.emplace_back(ColorImageView->GetHandle()); }
		for (auto& ResolveImageView : ResolveImageViews) { AttachmentViews.emplace_back(ResolveImageView->GetHandle()); }
		if (HasDepthStencil()) { AttachmentViews.emplace_back(DepthStencilImageView->GetHandle()); }

		VkFramebufferCreateInfo CreateInfo
		{
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext			 = nullptr,
			.flags			 = 0x0,
			.renderPass		 = Owner,
			.attachmentCount = UInt32(AttachmentViews.size()),
			.pAttachments	 = AttachmentViews.data(),
			.width  = _OwnerLayout->GetRenderAreaExtent3D().width,
			.height = _OwnerLayout->GetRenderAreaExtent3D().height,
			.layers = _OwnerLayout->GetRenderAreaExtent3D().depth,
		};

		if (vkCreateFramebuffer(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Framebuffer!"); }
	}

	FVulkanFramebuffer::
	~FVulkanFramebuffer()
	{	
		ColorImageViews.clear();
		ResolveImageViews.clear();
		DepthStencilImageView.reset();

		vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime