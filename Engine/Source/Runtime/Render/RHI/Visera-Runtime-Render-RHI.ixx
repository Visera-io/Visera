module;
#include <Visera.h>
export module Visera.Runtime.Render.RHI;

import Visera.Runtime.Render.RHI.Vulkan;
import Visera.Core.Log;

VISERA_PUBLIC_MODULE

#define CALL static inline auto

class Render;

class RHI
{
	friend class Render;
public:
	using Semaphore			= Vulkan::Semaphore;
	using CommandPool		= Vulkan::CommandPool;
	using CommandBuffer		= Vulkan::CommandPool::CommandBuffer;
	using Fence				= Vulkan::Fence;
	using Shader			= Vulkan::Shader;
	using RenderPass		= Vulkan::RenderPass;
	using Buffer			= VulkanAllocator::Buffer;

	using EShaderStage		= Vulkan::EShaderStage;
	using EAccessibility	= Vulkan::EAccessibility;
	using EPipelineStage	= Vulkan::EPipelineStage;
	using EMemoryUsage		= Vulkan::EMemoryUsage;
	using EBufferUsage		= Vulkan::EBufferUsage;
	using EImageLayout		= Vulkan::EImageLayout;
	using EPipelineStage	= Vulkan::EPipelineStage;
	using EAttachmentIO		= Vulkan::EAttachmentIO;

	using SwapchainRecreateSignal = VulkanSwapchain::RecreateSignal;

	class CommandContext;

public:
	CALL RegisterCommandContext(const String& Name, EPipelineStage Deadline) -> void;
	CALL SearchCommandContext(StringView Name)	-> WeakPtr<CommandContext>;

	CALL CreateBuffer(const Buffer::CreateInfo& _CreateInfo) -> SharedPtr<Buffer> { return VulkanAPI.Allocator.CreateBuffer(_CreateInfo); }
	CALL CreateFence()						-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(); }
	CALL CreateSignaledFence()				-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(true); }
	CALL CreateSemaphore()					-> SharedPtr<Semaphore> { return CreateSharedPtr<Semaphore>(); }
	CALL CreateShader(EShaderStage Stage, const Array<Byte>& ShadingCode) -> SharedPtr<Shader> { return CreateSharedPtr<VulkanShader>(Stage, ShadingCode);}

	CALL WaitIdle() -> void { VulkanAPI.Device.WaitIdle(); }
public:
	class CommandContext
	{
		friend class RHI;
		friend class Render; //[TODO] Remove
	public:
		StringView						 Name;
		SharedPtr<CommandBuffer>		 Commands;

	private:
		void Create(StringView Name, EPipelineStage Deadline, SharedPtr<Fence> SignalFence);
		void Destroy() noexcept {};

	private:
		EPipelineStage					 Deadline;
		Array<SharedPtr<CommandContext>> Dependencies;
		Semaphore						 Semaphore_Compeleted;
		SharedPtr<Fence>				 Fence_Executing;
	};


private:
	static inline Vulkan VulkanAPI;

	struct Frame
	{
		Fence		Fence_Rendering{ True };
		Semaphore	Semaphore_ReadyToRender;
		Semaphore	Semaphore_ReadyToPresent;
		HashMap<String, SharedPtr<CommandContext>> CommandContexts;
	};
	static inline Array<Frame> Frames;

	CALL GetCurrentFrame() -> Frame& { return Frames[VulkanAPI.Swapchain.GetCursor()]; }

	static inline void
	WaitForCurrentFrame() throw(SwapchainRecreateSignal)
	{
		auto& CurrentFrame = GetCurrentFrame();
		CurrentFrame.Fence_Rendering.Wait();
		VulkanAPI.Swapchain.WaitForCurrentImage(CurrentFrame.Semaphore_ReadyToRender, nullptr);
		CurrentFrame.Fence_Rendering.Lock(); //Reset to Unsignaled (Lock)
	}

	static inline void
	PresentCurrentFrame()	throw(SwapchainRecreateSignal)
	{
		auto& CurrentFrame = GetCurrentFrame();
		VulkanAPI.Swapchain.PresentCurrentImage(CurrentFrame.Semaphore_ReadyToPresent);
	}

private:
	static inline VulkanCommandPool ResetableGraphicsCommandPool{};
	static inline VulkanCommandPool TransientGraphicsCommandPool{};

private:
	RHI() noexcept = default;
	static void
	Bootstrap()
	{
		VulkanAPI.Bootstrap();
		ResetableGraphicsCommandPool.Create(VulkanDevice::QueueFamilyType::Graphics, VulkanCommandPool::PoolType::Resetable);
		TransientGraphicsCommandPool.Create(VulkanDevice::QueueFamilyType::Graphics, VulkanCommandPool::PoolType::Transient);
		Frames.resize(VulkanAPI.Swapchain.GetSize());
	}
	static void
	Terminate()
	{
		WaitIdle();
		Frames.clear();
		TransientGraphicsCommandPool.Destroy();
		ResetableGraphicsCommandPool.Destroy();
		VulkanAPI.Terminate();
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

VISERA_MODULE_END