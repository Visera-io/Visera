module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassResource;
import :Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanRenderPass;
	class FVulkanFramebuffer;

	class FVulkanRenderPassResource
	{
		VE_NOT_COPYABLE(FVulkanRenderPassResource);
		friend class FVulkanRenderPass;
		friend class FVulkanFramebuffer;
	public:
		void AddColorImage(SharedPtr<FVulkanImage> _ColorImage);
		void AddDepthImage(SharedPtr<FVulkanImage> _DepthImage);
		auto GetTotalImageCount()	const	-> UInt8 { return GetColorImageCount() + GetResolveImageCount() + (HasDepthImage()? 1 : 0); }
		auto GetColorImageCount()	const	-> UInt8 { return ColorImages.size(); }
		auto GetResolveImageCount() const	-> UInt8 { return ResolveImages.size(); }
		Bool HasDepthImage() const { return DepthImage != nullptr; }

		auto Clone() const -> SharedPtr<FVulkanRenderPassResource>;

		FVulkanRenderPassResource() = default;
		FVulkanRenderPassResource(const Array<SharedPtr<FVulkanImage>>& _ColorImages, SharedPtr<FVulkanImage> _DepthImage);

	private:
		Array<SharedPtr<FVulkanImage>>		ColorImages;
		Array<SharedPtr<FVulkanImage>>		ResolveImages;
		SharedPtr<FVulkanImage>				DepthImage;
	};

	void FVulkanRenderPassResource::
	AddColorImage(SharedPtr<FVulkanImage> _ColorImage)
	{
		VE_ASSERT(AutoCast(EVulkanImageAspect::Color & _ColorImage->GetAspects()) != False);
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
		ColorImages.emplace_back(std::move(_ColorImage));
	}

	void FVulkanRenderPassResource::
	AddDepthImage(SharedPtr<FVulkanImage> _DepthImage)
	{
		VE_ASSERT(AutoCast(EVulkanImageAspect::Depth & _DepthImage->GetAspects()) != False);
		if (!HasDepthImage())
		{  
			DepthImage = std::move(_DepthImage);
		} 
		else { throw SRuntimeError("Cannnot add more than one depth image!"); }
	}

	FVulkanRenderPassResource::
	FVulkanRenderPassResource(const Array<SharedPtr<FVulkanImage>>&	_ColorImages,
						 SharedPtr<FVulkanImage>					_DepthImage)
		:ColorImages {_ColorImages},
		 DepthImage  {std::move(_DepthImage)}
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
				EVulkanImageAspect::Color,
				EVulkanImageUsage::ColorAttachment | EVulkanImageUsage::InputAttachment,
				EVulkanImageTiling::Optimal,
				EVulkanSampleRate::X1);
		}
	}

	SharedPtr<FVulkanRenderPassResource> FVulkanRenderPassResource::
	Clone() const
	{
		auto Result = CreateSharedPtr<FVulkanRenderPassResource>();
		Result->ColorImages.resize(GetColorImageCount());
		Result->ResolveImages.resize(GetResolveImageCount());
		for (UInt8 Idx = 0; Idx < GetColorImageCount(); ++Idx)
		{
			Result->ColorImages[Idx]   = ColorImages[Idx]->Clone();
			Result->ResolveImages[Idx] = ResolveImages[Idx]->Clone();
		}
		Result->DepthImage = DepthImage->Clone();
		return Result;
	}

} } // namespace VE::Runtime