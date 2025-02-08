module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderTargets;
import :Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderTargets
	{
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		Bool HasDepthImage() const { return DepthImage != nullptr; }

		FVulkanRenderTargets() = delete;
		FVulkanRenderTargets(const Array<SharedPtr<FVulkanImage>>& _ColorImages, SharedPtr<FVulkanImage> _DepthImage = nullptr);

	private:
		Array<SharedPtr<FVulkanImage>>	ColorImages;
		Array<SharedPtr<FVulkanImage>>	ResolveImages;
		SharedPtr<FVulkanImage>			DepthImage;
	};

	FVulkanRenderTargets::
	FVulkanRenderTargets(const Array<SharedPtr<FVulkanImage>>&	_ColorImages,
						 SharedPtr<FVulkanImage>				_DepthImage/* = nullptr*/)
		:ColorImages {_ColorImages},
		 DepthImage  {_DepthImage}
	{
		if (ColorImages.empty())
		{ throw SRuntimeError("Failed to create Vulkan Render Targets! -- No Color Images."); }

		// Create Resolve Images for each Color Image
		ResolveImages.resize(ColorImages.size());
		for (UInt8 Idx = 0; Idx < ColorImages.size(); ++Idx)
		{
			ResolveImages[Idx] = GVulkan->Allocator->CreateImage(
				ColorImages[Idx]->GetType(),
				ColorImages[Idx]->GetExtent(),
				ColorImages[Idx]->GetFormat(),
				EImageAspect::Color,
				EImageUsage::ColorAttachment | EImageUsage::InputAttachment,
				EImageTiling::Optimal,
				ESampleRate::X1);
		}
	}

} } // namespace VE::Runtime