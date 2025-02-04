module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:RenderPassLayout;

import :Enums;
import :RenderPipeline;

export namespace VE { namespace Runtime
{

	class FVulkanRenderPass;

	class FVulkanRenderPassLayout
	{
		friend class FVulkanRenderPass;
	public:
		auto GetOffset2D() const -> const VkOffset2D& { return Offset.Offset2D; }
		auto GetOffset3D() const -> const VkOffset3D& { return Offset.Offset3D; }
		auto GetExtent2D() const -> const VkExtent2D& { return Extent.Extent2D; }
		auto GetExtent3D() const -> const VkExtent3D& { return Extent.Extent3D; }

		Bool HasDepth()    const  { return DepthImageReference.has_value(); }
		Bool HasStencil()  const  { return StencilImageReference.has_value(); }

		FVulkanRenderPassLayout()  noexcept = default;
		~FVulkanRenderPassLayout() noexcept = default;

	private:
		Array<VkAttachmentReference>	InputImageReferences;    // Input Image References from Previous Subpasses.
		Array<UInt32>					PreserveImageReferences; // Const Image References Used in Subpasses.
		Array<VkAttachmentReference>	ColorImageReferences;
		Array<VkAttachmentReference>	ResolveImageReferences;
		Optional<VkAttachmentReference> DepthImageReference;
		Optional<VkAttachmentReference> StencilImageReference;

		Array<VkAttachmentDescription>			AttachmentDescriptions;
		VkAttachmentDescriptionStencilLayout	StencilDescription;
		
		UInt8 ClearValueCount = 0;
		UInt8 SampleCount     = 0;

		union
		{
			VkOffset3D Offset3D;
			VkOffset2D Offset2D;
		} Offset;

		union
		{
			VkExtent3D	Extent3D;
			VkExtent2D	Extent2D;
		} Extent;
	};

} } // namespace VE::Runtime