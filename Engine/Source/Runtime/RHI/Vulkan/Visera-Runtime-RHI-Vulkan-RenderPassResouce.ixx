module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassResource;
import :Common;
import :Allocator;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	enum { MaxSimultaneousRenderTargets = 8 };
	enum class ESystemRT : UInt8
	{
		Color = 0,
		Depth = 1,
	};

	class FVulkanRenderPassResource
	{
	public:
		auto GetSystemRT(ESystemRT _Type) -> SharedPtr<const FVulkanImage>;

	private:
		Array<SharedPtr<FVulkanImage>>	ColorImages;
		Array<SharedPtr<FVulkanImage>>	ResolveColorImages; // MSAA Result
		SharedPtr<FVulkanImage>			DepthImage;
	};

	SharedPtr<const FVulkanImage> FVulkanRenderPassResource::
	GetSystemRT(ESystemRT _Type)
	{
		switch (_Type)
		{
		case ESystemRT::Color:
			return ColorImages[UInt8(ESystemRT::Color)];
		case ESystemRT::Depth:
			return DepthImage;
		default:
			throw SRuntimeError("Unknown System Render Target Type!");
		}
	};

} } // namespace VE::Runtime