module;
#include <Visera>

export module Visera.App.Render;

import Visera.App.Base;
import Visera.Engine.Runtime.Render;

export namespace VISERA_APP_NAMESPACE
{
	#define CALL auto inline

	using RenderPass = VE::Runtime::RHI::RenderPass;

	using Shader		= VE::Runtime::RHI::Shader;
	using ShaderStage	= VE::Runtime::RHI::ShaderStage;
	using ShaderStages	= VE::Runtime::RHI::ShaderStages;
	CALL CreateShader(ShaderStages Stages, const Array<Byte>& ShadingCode) -> SharedPtr<Shader> { return VE::Runtime::RHI::CreateShader(Stages, ShadingCode); }

	using CommandBuffer = VE::Runtime::VulkanCommandPool::CommandBuffer;
	CALL CreateDrawCalls			(CommandBuffer::Level Level = CommandBuffer::Level::Primary) -> SharedPtr<CommandBuffer> { return VE::Runtime::RHI::GetResetableGraphicsCommandPool().Allocate(Level); }
	CALL CreateOneTimeDrawCalls		(CommandBuffer::Level Level = CommandBuffer::Level::Primary) -> SharedPtr<CommandBuffer> { return VE::Runtime::RHI::GetTransientGraphicsCommandPool().Allocate(Level); }
	
	using Fence			= VE::Runtime::VulkanFence;
	CALL CreateFence				()	-> SharedPtr<Fence>		{ return VE::Runtime::RHI::CreateFence(); }
	CALL CreateSignaledFence		()	-> SharedPtr<Fence>		{ return VE::Runtime::RHI::CreateSignaledFence(); }

	using Semaphore		= VE::Runtime::VulkanSemaphore;
	CALL CreateSemaphore			()	-> SharedPtr<Semaphore> { return VE::Runtime::RHI::CreateSemaphore(); }
	CALL CreateSignaledSemaphore	()	-> SharedPtr<Semaphore> { return VE::Runtime::RHI::CreateSignaledSemaphore(); }

} // namespace VISERA_APP_NAMESPACE
