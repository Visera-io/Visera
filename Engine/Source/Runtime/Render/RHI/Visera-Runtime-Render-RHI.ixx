module;
#include <Visera.h>
export module Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.RHI.Vulkan;
import Visera.Runtime.Render.RHI.RenderPass;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class Render;

	class RHI
	{
		VE_MODULE_MANAGER_CLASS(RHI);
		friend class Render;
	public:
		using FSemaphore		= FVulkanSemaphore;
		using FCommandPool		= FVulkanCommandPool;
		using FCommandBuffer	= FVulkanCommandPool::FVulkanCommandBuffer;
		using FFence			= FVulkanFence;
		using FShader			= FVulkanShader;
		using FBuffer			= FVulkanAllocator::Buffer;
		using FFramebuffer		= FVulkanFramebuffer;
		using FRenderPipeline   = FVulkanRenderPipeline;
		using FPipelineLayout   = FVulkanPipelineLayout;

		using ESampleRate		= ESampleRate;
		using EQueueFamily		= EQueueFamily;
		using ECommandPool		= ECommandPool;
		using ECommandLevel		= ECommandLevel;
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
		VE_API RegisterCommandContext(const String& Name, EPipelineStage Deadline) -> void;
		VE_API SearchCommandContext(StringView Name)	-> WeakPtr<CommandContext>;

		VE_API CreateBuffer(const FBuffer::CreateInfo& _CreateInfo) -> SharedPtr<FBuffer> { return Vulkan->Allocator.CreateBuffer(_CreateInfo); }
		VE_API CreateFence()				-> SharedPtr<FFence>		{ return CreateSharedPtr<FFence>(); }
		VE_API CreateSignaledFence()		-> SharedPtr<FFence>		{ return CreateSharedPtr<FFence>(true); }
		VE_API CreateSemaphore()			-> SharedPtr<FSemaphore> { return CreateSharedPtr<FSemaphore>(); }
		VE_API CreateShader(EShaderStage Stage, const Array<Byte>& ShadingCode) -> SharedPtr<FShader> { return CreateSharedPtr<FVulkanShader>(Stage, ShadingCode);}

		VE_API WaitIdle() -> void { Vulkan->Device.WaitIdle(); }

		VE_API GetAPI() -> const FVulkan* { return Vulkan; }

	public:
		class CommandContext
		{
			friend class RHI;
		public:
			StringView						 Name;
			SharedPtr<FCommandBuffer>		 Commands;

		//private:
			void Create(StringView Name, EPipelineStage Deadline, SharedPtr<FFence> SignalFence);
			void Destroy() noexcept {};

		//private:
			EPipelineStage					 Deadline;
			Array<SharedPtr<CommandContext>> Dependencies;
			FSemaphore						 Semaphore_Compeleted;
			SharedPtr<FFence>				 Fence_Executing;
		};


	private:	
		static inline FVulkan*	 Vulkan;

		struct Frame
		{
			FFence		Fence_Rendering{ True };
			FSemaphore	Semaphore_ReadyToRender;
			FSemaphore	Semaphore_ReadyToPresent;
			HashMap<String, SharedPtr<CommandContext>> CommandContexts;
		};
		static inline Array<Frame> Frames;

		VE_API GetCurrentFrame() -> Frame& { return Frames[Vulkan->Swapchain.GetCursor()]; }

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

	private:
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
			SharedPtr<FFence>		SignalFence)
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
			NewCommandContext->Commands = ResetableGraphicsCommandPool.Allocate(ECommandLevel::Primary);
		}
		auto& CurrentFrame = GetCurrentFrame();
	}

	WeakPtr<RHI::CommandContext> RHI::
	SearchCommandContext(StringView Name)
	{
		auto& CurrentFrame = GetCurrentFrame();
		auto Result = CurrentFrame.CommandContexts.find(Name.data());

		if(Result == CurrentFrame.CommandContexts.end())
		{ throw SRuntimeError(Text("Failed to search Command Context ({})", Name)); }

		return Result->second;
	}

} } // namespace VE::Runtime