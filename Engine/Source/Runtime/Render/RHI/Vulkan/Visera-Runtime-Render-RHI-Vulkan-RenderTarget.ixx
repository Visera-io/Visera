module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderTarget;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderTarget
	{
		VE_NOT_COPYABLE(FVulkanRenderTarget);
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		void AddColorImage(SharedPtr<FVulkanImage> _ColorImage);
		void AddDepthImage(SharedPtr<FVulkanImage> _DepthImage);

		auto GetColorImage(UInt8 _Index)	-> SharedPtr<FVulkanImage> { VE_ASSERT(_Index < ColorImages.size()); return ColorImages[_Index]; };
		auto GetTotalImageCount()	const	-> UInt8 { return GetColorImageCount() + GetResolveImageCount() + (HasDepthImage()? 1 : 0); }
		auto GetColorImageCount()	const	-> UInt8 { return ColorImages.size(); }
		auto GetResolveImageCount() const	-> UInt8 { return ResolveImages.size(); }

		Bool HasDepthImage() const { return DepthImage != nullptr; }

		auto Clone() const -> SharedPtr<FVulkanRenderTarget>;

		FVulkanRenderTarget() = default;
		FVulkanRenderTarget(const Array<SharedPtr<FVulkanImage>>& _ColorImages, SharedPtr<FVulkanImage> _DepthImage);
		FVulkanRenderTarget(FVulkanRenderTarget&& _Another) = default;
		FVulkanRenderTarget& operator=(FVulkanRenderTarget&& _Another) = default;

	private:
		Array<SharedPtr<FVulkanImage>>		ColorImages;
		Array<SharedPtr<FVulkanImage>>		ResolveImages;
		SharedPtr<FVulkanImage>				DepthImage;
	};

	void FVulkanRenderTarget::
	AddColorImage(SharedPtr<FVulkanImage> _ColorImage)
	{
		VE_ASSERT(AutoCast(EVulkanImageAspect::Color & _ColorImage->GetAspects()) != False);
		auto& NewColorImage = ColorImages.emplace_back(std::move(_ColorImage));
		if (NewColorImage->EnabledMSAA())
		{
			ResolveImages.emplace_back(std::move(
				GVulkan->Allocator->CreateImage(
					_ColorImage->GetType(),
					_ColorImage->GetExtent(),
					_ColorImage->GetFormat(),
					EVulkanImageAspect::Color,
					EVulkanImageUsage::ColorAttachment | EVulkanImageUsage::InputAttachment,
					EVulkanImageTiling::Optimal,
					EVulkanSampleRate::X1)
			));
		}
	}

	void FVulkanRenderTarget::
	AddDepthImage(SharedPtr<FVulkanImage> _DepthImage)
	{
		VE_ASSERT(AutoCast(EVulkanImageAspect::Depth & _DepthImage->GetAspects()) != False);
		if (!HasDepthImage())
		{  
			DepthImage = std::move(_DepthImage);
		} 
		else { throw SRuntimeError("Cannnot add more than one depth image!"); }
	}

	FVulkanRenderTarget::
	FVulkanRenderTarget(const Array<SharedPtr<FVulkanImage>>&	_ColorImages,
						 SharedPtr<FVulkanImage>				_DepthImage)
		:ColorImages {_ColorImages},
		 DepthImage  {std::move(_DepthImage)}
	{
		if (ColorImages.empty())
		{ throw SRuntimeError("Failed to create Vulkan Render Targets! -- No Color Images."); }

		// Create Resolve Images for each Color Image
		if (ColorImages.front()->EnabledMSAA())
		{
			//[FIXME]: Some images may not need to MSAA
			ResolveImages.resize(ColorImages.size());
			for (UInt8 Idx = 0; Idx < ColorImages.size(); ++Idx)
			{
				ResolveImages[Idx] = GVulkan->Allocator->CreateImage(
					ColorImages[Idx]->GetType(),
					ColorImages[Idx]->GetExtent(),
					ColorImages[Idx]->GetFormat(),
					EVulkanImageAspect::Color,
					EVulkanImageUsage::ColorAttachment | EVulkanImageUsage::InputAttachment,
					EVulkanImageTiling::Optimal,
					EVulkanSampleRate::X1);
			}
		}
	}

	SharedPtr<FVulkanRenderTarget> FVulkanRenderTarget::
	Clone() const
	{
		auto Result = CreateSharedPtr<FVulkanRenderTarget>();
		Result->ColorImages.resize(GetColorImageCount());
		Result->ResolveImages.resize(GetResolveImageCount());
		for (UInt8 Idx = 0; Idx < GetColorImageCount(); ++Idx)
		{
			Result->ColorImages[Idx]   = ColorImages[Idx]->Clone();
		}
		for (UInt8 Idx = 0; Idx < GetResolveImageCount(); ++Idx)
		{
			Result->ResolveImages[Idx] = ResolveImages[Idx]->Clone();
		}
		Result->DepthImage = DepthImage->Clone();
		return Result;
	}

} // namespace VE