module;
#include <Visera.h>

export module Visera.App.Render;

import Visera.App.Base;
import Visera.Runtime.Render.RHI;

export namespace VISERA_APP_NAMESPACE
{
	#define CALL auto inline
	using RHI = VE::RHI;

	using RenderPass		= VE::RHI::RenderPass;
	/*using PipelineStages	= VE::RHI::PipelineStages;
	using AccessPermissions	= VE::RHI::AccessPermissions;
	using ImageLayouts		= VE::RHI::ImageLayouts;
	using AttachmentIO		= VE::RHI::AttachmentIO;

	using Shader		= SharedPtr<VE::RHI::Shader>;
	using ShaderStages	= VE::RHI::ShaderStages;
	CALL CreateShader(ShaderStages Stage, const Array<Byte>& ShadingCode) -> Shader { return VE::RHI::CreateShader(Stage, ShadingCode); }
	*/
	using CommandBuffer			= SharedPtr<VE::RHI::CommandPool::CommandBuffer>;
	using CommandBufferLevel	= VE::RHI::CommandBuffer::Level;
	//CALL CreateDrawCalls			(CommandBufferLevel Level = CommandBufferLevel::Primary) -> CommandBuffer { return VE::RHI::GetResetableGraphicsCommandPool().Allocate(Level); }
	//CALL CreateOneTimeDrawCalls		(CommandBufferLevel Level = CommandBufferLevel::Primary) -> CommandBuffer { return VE::RHI::GetTransientGraphicsCommandPool().Allocate(Level); }

	using Fence			= SharedPtr<VE::VulkanFence>;
	CALL CreateFence				()	-> Fence	{ return VE::RHI::CreateFence(); }
	CALL CreateSignaledFence		()	-> Fence	{ return VE::RHI::CreateSignaledFence(); }

	using Semaphore		= SharedPtr<VE::RHI::Semaphore>;
	CALL CreateSemaphore			()	-> Semaphore { return VE::RHI::CreateSemaphore(); }
	CALL CreateSignaledSemaphore	()	-> Semaphore { return VE::RHI::CreateSignaledSemaphore(); }

} // namespace VISERA_APP_NAMESPACE
