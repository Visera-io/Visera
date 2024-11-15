module;
#include <Visera>

export module Visera.App.Render;

import Visera.App.Base;
import Visera.Engine.Runtime.Render.RHI;

export namespace VISERA_APP_NAMESPACE
{
	#define CALL auto inline
	using RHI = VE::Runtime::RHI;

	using RenderPass		= VE::Runtime::RHI::RenderPass;
	/*using PipelineStages	= VE::Runtime::RHI::PipelineStages;
	using AccessPermissions	= VE::Runtime::RHI::AccessPermissions;
	using ImageLayouts		= VE::Runtime::RHI::ImageLayouts;
	using AttachmentIO		= VE::Runtime::RHI::AttachmentIO;

	using Shader		= SharedPtr<VE::Runtime::RHI::Shader>;
	using ShaderStages	= VE::Runtime::RHI::ShaderStages;
	CALL CreateShader(ShaderStages Stage, const Array<Byte>& ShadingCode) -> Shader { return VE::Runtime::RHI::CreateShader(Stage, ShadingCode); }
	*/
	using CommandBuffer			= SharedPtr<VE::Runtime::RHI::CommandPool::CommandBuffer>;
	using CommandBufferLevel	= VE::Runtime::RHI::CommandBuffer::Level;
	//CALL CreateDrawCalls			(CommandBufferLevel Level = CommandBufferLevel::Primary) -> CommandBuffer { return VE::Runtime::RHI::GetResetableGraphicsCommandPool().Allocate(Level); }
	//CALL CreateOneTimeDrawCalls		(CommandBufferLevel Level = CommandBufferLevel::Primary) -> CommandBuffer { return VE::Runtime::RHI::GetTransientGraphicsCommandPool().Allocate(Level); }

	using Fence			= SharedPtr<VE::Runtime::VulkanFence>;
	CALL CreateFence				()	-> Fence	{ return VE::Runtime::RHI::CreateFence(); }
	CALL CreateSignaledFence		()	-> Fence	{ return VE::Runtime::RHI::CreateSignaledFence(); }

	using Semaphore		= SharedPtr<VE::Runtime::RHI::Semaphore>;
	CALL CreateSemaphore			()	-> Semaphore { return VE::Runtime::RHI::CreateSemaphore(); }
	CALL CreateSignaledSemaphore	()	-> Semaphore { return VE::Runtime::RHI::CreateSignaledSemaphore(); }

} // namespace VISERA_APP_NAMESPACE
