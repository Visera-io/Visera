module;
#include <Visera.h>
export module Visera.Editor.UI:UIRenderPass;

import Visera.Runtime.RHI;

export namespace VE { namespace Editor
{
	using namespace VE::Runtime;

	class FUIRenderPass : public RHI::FRenderPass
	{
	public:
		FUIRenderPass() : RHI::FRenderPass{ ERenderPassType::Background }
		{
			Subpasses.emplace_back(FSubpass
			{ 
				.ColorImageReferences = {0},
				.SrcStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.SrcStageAccess = RHI::EAccessibility::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccessibility::W_ColorAttachment,
				.bExternalSubpass = True
			});
		}

	private:


	};

} } // namespace VE::Editor