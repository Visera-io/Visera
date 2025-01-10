module;
#include <Visera.h>
export module Visera.Runtime.RHI;

import Visera.Runtime.RHI.Vulkan;
import Visera.Core.Log;

export namespace VE { namespace Runtime
{
#define CALL static inline auto

class UI;

class RHI
{
	friend class UI;
public:
	using Semaphore			= FVulkanSemaphore;
	using CommandPool		= FVulkanCommandPool;
	using CommandBuffer		= FVulkanCommandPool::CommandBuffer;
	using Fence				= FVulkanFence;
	using Shader			= FVulkanShader;
	using RenderPass		= FVulkanRenderPass;
	using Buffer			= FVulkanAllocator::Buffer;

	using ESampleRate		= ESampleRate;
	using EQueueFamily		= EQueueFamily;
	using ECommandPool		= ECommandPool;
	using EShaderStage		= EShaderStage;
	using EAccessibility	= EAccessibility;
	using EPipelineStage	= EPipelineStage;
	using EMemoryUsage		= EMemoryUsage;
	using EBufferUsage		= EBufferUsage;
	using EImageLayout		= EImageLayout;
	using EPipelineStage	= EPipelineStage;
	using EAttachmentIO		= EAttachmentIO;

	using SwapchainRecreateSignal = FVulkanSwapchain::RecreateSignal;

	class CommandContext;

public:
	CALL RegisterCommandContext(const String& Name, EPipelineStage Deadline) -> void;
	CALL SearchCommandContext(StringView Name)	-> WeakPtr<CommandContext>;

	CALL CreateBuffer(const Buffer::CreateInfo& _CreateInfo) -> SharedPtr<Buffer> { return Vulkan->Allocator.CreateBuffer(_CreateInfo); }
	CALL CreateFence()				-> SharedPtr<Fence>		{ return CreateSharedPtr<Fence>(); }
	CALL CreateSignaledFence()		-> SharedPtr<Fence>		{ return CreateSharedPtr<Fence>(true); }
	CALL CreateSemaphore()			-> SharedPtr<Semaphore> { return CreateSharedPtr<Semaphore>(); }
	CALL CreateShader(EShaderStage Stage, const Array<Byte>& ShadingCode) -> SharedPtr<Shader> { return CreateSharedPtr<FVulkanShader>(Stage, ShadingCode);}

	CALL WaitIdle() -> void { Vulkan->Device.WaitIdle(); }

//private:
	static inline FVulkan*	 Vulkan;
	CALL GetAPI() -> const FVulkan* { return Vulkan; }

public:
	class CommandContext
	{
		friend class RHI;
	public:
		StringView						 Name;
		SharedPtr<CommandBuffer>		 Commands;

	//private:
		void Create(StringView Name, EPipelineStage Deadline, SharedPtr<Fence> SignalFence);
		void Destroy() noexcept {};

	//private:
		EPipelineStage					 Deadline;
		Array<SharedPtr<CommandContext>> Dependencies;
		Semaphore						 Semaphore_Compeleted;
		SharedPtr<Fence>				 Fence_Executing;
	};


//private:
	struct Frame
	{
		Fence		Fence_Rendering{ True };
		Semaphore	Semaphore_ReadyToRender;
		Semaphore	Semaphore_ReadyToPresent;
		HashMap<String, SharedPtr<CommandContext>> CommandContexts;
	};
	static inline Array<Frame> Frames;

	CALL GetCurrentFrame() -> Frame& { return Frames[Vulkan->Swapchain.GetCursor()]; }

	static inline void
	WaitForCurrentFrame() throw(SwapchainRecreateSignal)
	{
		auto& CurrentFrame = GetCurrentFrame();
		CurrentFrame.Fence_Rendering.Wait();
		Vulkan->Swapchain.WaitForCurrentImage(CurrentFrame.Semaphore_ReadyToRender, nullptr);
		CurrentFrame.Fence_Rendering.Lock(); //Reset to Unsignaled (Lock)
	}

	static inline void
	PresentCurrentFrame()	throw(SwapchainRecreateSignal)
	{
		auto& CurrentFrame = GetCurrentFrame();
		Vulkan->Swapchain.PresentCurrentImage(CurrentFrame.Semaphore_ReadyToPresent);
	}

//private:
	static inline FVulkanCommandPool ResetableGraphicsCommandPool{};
	static inline FVulkanCommandPool TransientGraphicsCommandPool{};

//private:
	RHI() noexcept = default;
	static void
	Bootstrap()
	{
		Vulkan = new FVulkan();
		ResetableGraphicsCommandPool.Create(EQueueFamily::Graphics, ECommandPool::Resetable);
		TransientGraphicsCommandPool.Create(EQueueFamily::Graphics, ECommandPool::Transient);
		Frames.resize(Vulkan->Swapchain.GetSize());
	}
	static void
	Terminate()
	{
		WaitIdle();
		Frames.clear();
		TransientGraphicsCommandPool.Destroy();
		ResetableGraphicsCommandPool.Destroy();
		delete Vulkan;
	}
};

void RHI::CommandContext::
Create(	StringView				Name,
		EPipelineStage			Deadline,
		SharedPtr<Fence>		SignalFence)
{
	this->Name				= std::move(Name);
	this->Deadline			= Deadline;
	this->Fence_Executing	= std::move(SignalFence);
}

void RHI::
RegisterCommandContext(const String& Name, EPipelineStage Deadline)
{
	for (auto& Frame : Frames)
	{
		VE_ASSERT(!Frame.CommandContexts.count(Name));
		auto& NewCommandContext = Frame.CommandContexts[Name];
		NewCommandContext = CreateSharedPtr<CommandContext>();
		NewCommandContext->Create(Name, Deadline, nullptr);
		//[FIXME]: Create Buffer based on Create(X_para)
		NewCommandContext->Commands = ResetableGraphicsCommandPool.Allocate(CommandBuffer::Level::Primary);
	}
	auto& CurrentFrame = GetCurrentFrame();
}

WeakPtr<RHI::CommandContext> RHI::
SearchCommandContext(StringView Name)
{
	auto& CurrentFrame = GetCurrentFrame();
	auto Result = CurrentFrame.CommandContexts.find(Name.data());

	if(Result == CurrentFrame.CommandContexts.end())
	{ Log::Fatal(Text("Failed to search Command Context ({})", Name)); }

	return Result->second;
}

} } // namespace VE::Runtime