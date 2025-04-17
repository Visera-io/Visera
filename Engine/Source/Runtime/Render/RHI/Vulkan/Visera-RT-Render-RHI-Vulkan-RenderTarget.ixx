module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:RenderTarget;
#define VE_MODULE_NAME "Vulkan:RenderTarget"
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Allocator;

import Visera.Core.Log;

export namespace VE
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderTarget : public std::enable_shared_from_this<FVulkanRenderTarget>
	{
		VE_NOT_COPYABLE(FVulkanRenderTarget);
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		static inline auto
		Create() { return CreateSharedPtr<FVulkanRenderTarget>(); }
		auto inline
		AddColorImage(SharedPtr<FVulkanImage> _ColorImage) -> FVulkanRenderTarget*;
		auto inline
		AddDepthImage(SharedPtr<FVulkanImage> _DepthImage) -> FVulkanRenderTarget*;
		auto inline
		Confirm() { VE_ASSERT(!IsConfirmed()); bConfirmed = True; return shared_from_this();}
		Bool inline
		IsConfirmed() const { return bConfirmed; }

		auto GetColorImage(UInt8 _Index)	-> SharedPtr<FVulkanImage> { VE_ASSERT(_Index < ColorImages.size()); return ColorImages[_Index]; };
		auto GetTotalImageCount()	const	-> UInt8 { return GetColorImageCount() + GetResolveImageCount() + (HasDepthImage()? 1 : 0); }
		auto GetColorImageCount()	const	-> UInt8 { return ColorImages.size(); }
		auto GetResolveImageCount() const	-> UInt8 { return ResolveImages.size(); }

		Bool HasDepthImage() const { return DepthImage != nullptr; }

		auto Clone() const -> SharedPtr<FVulkanRenderTarget>;

		FVulkanRenderTarget() = default;
		FVulkanRenderTarget(FVulkanRenderTarget&& _Another) = default;
		FVulkanRenderTarget& operator=(FVulkanRenderTarget&& _Another) = default;

	private:
		Array<SharedPtr<FVulkanImage>>		ColorImages;
		Array<SharedPtr<FVulkanImage>>		ResolveImages;
		SharedPtr<FVulkanImage>				DepthImage;
		Bool bConfirmed = False;
	};

	FVulkanRenderTarget* FVulkanRenderTarget::
	AddColorImage(SharedPtr<FVulkanImage> _ColorImage)
	{
		VE_ASSERT(!IsConfirmed());
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
					EVulkanImageUsage::ColorAttachment | EVulkanImageUsage::InputAttachment)
			));
		}

		return this;
	}

	FVulkanRenderTarget* FVulkanRenderTarget::
	AddDepthImage(SharedPtr<FVulkanImage> _DepthImage)
	{
		VE_ASSERT(!IsConfirmed());
		VE_ASSERT(AutoCast(EVulkanImageAspect::Depth & _DepthImage->GetAspects()) != False);
		if (!HasDepthImage())
		{  
			DepthImage = std::move(_DepthImage);
		} 
		else { VE_LOG_FATAL("Cannnot add more than one depth image!"); }

		return this;
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