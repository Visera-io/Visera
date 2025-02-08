module;

#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Framebuffer;
import :Common;
import :Device;
import :RenderPassResource;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;

	class FVulkanFramebuffer
	{
		friend class FVulkanRenderPass;
	public:
		Bool HasDepthImage() const { return RenderTargets->HasDepthImage(); }
	
	private:
		VkFramebuffer						Handle{ VK_NULL_HANDLE };
		FVulkanExtent3D						Extent;
		SharedPtr<FVulkanRenderPassResource>RenderTargets;
		Array<VkImageView>					RenderTargetViews;
		//[TODO]: Shading Rate Image & VkAttachmentDescriptionStencilLayout StencilDescription;
	
	private:
		// Created in RenderPass
		void Create(const VkRenderPass& _Owner, const FVulkanExtent3D& _Extent, SharedPtr<FVulkanRenderPassResource> _RenderTargets);
		void Destroy();

	public:
		FVulkanFramebuffer() = default;
		~FVulkanFramebuffer() { Destroy(); }
	};

	void FVulkanFramebuffer::
	Create(const VkRenderPass& _Owner,
		const FVulkanExtent3D& _Extent,
		SharedPtr<FVulkanRenderPassResource> _RenderTargets)
	{
		Extent = _Extent;
		RenderTargets = std::move(_RenderTargets);

		RenderTargetViews.reserve(RenderTargets->GetTotalImageCount());
		for (const auto& ColorImage : RenderTargets->ColorImages)
		{
			RenderTargetViews.push_back(ColorImage->CreateImageView()->Release());
		}
		for (const auto& ResolveImage : RenderTargets->ResolveImages)
		{
			RenderTargetViews.push_back(ResolveImage->CreateImageView()->Release());
		}
		if (HasDepthImage())
		{ RenderTargetViews.push_back(RenderTargets->DepthImage->CreateImageView()->Release()); }
	
		VE_ASSERT(RenderTargetViews.size() % 1 == 1); // Currently, Visera's each renderpass must have 2N + 1 attachments.
		VkFramebufferCreateInfo CreateInfo
		{
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext			 = nullptr,
			.flags			 = 0x0,
			.renderPass		 = _Owner,
			.attachmentCount = UInt32(RenderTargetViews.size()),
			.pAttachments	 = RenderTargetViews.data(),
			.width  = Extent.width,
			.height = Extent.height,
			.layers = Extent.depth,
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
		if (Handle != VK_NULL_HANDLE)
		{
			for (auto& RenderTargetView : RenderTargetViews)
			{
				vkDestroyImageView(GVulkan->Device->GetHandle(), RenderTargetView, GVulkan->AllocationCallbacks);
			}
			vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}

} } // namespace VE::Runtime