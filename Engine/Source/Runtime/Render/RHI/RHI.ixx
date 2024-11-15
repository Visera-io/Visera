module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI;
import Visera.Engine.Runtime.Render.RHI.Vulkan;
import Visera.Engine.Core.Log;

export namespace VE { namespace Runtime
{	
	#define ENUM_BIT(Flag, Bit) static constexpr VkFlags Flag = Bit;
	#define CALL static inline auto

	class Render;

	class RHI
	{
		friend class Render;
	public:
		using Semaphore			= VulkanSemaphore;
		using CommandPool		= VulkanCommandPool;
		using CommandBuffer		= CommandPool::CommandBuffer;
		using Fence				= VulkanFence;
		using Shader			= VulkanShader;
		using Swapchain			= VulkanSwapchain;
		using ShaderStages		= VulkanShaderStages::Option;
		using RenderPass		= VulkanRenderPass;
		using AccessPermissions	= VulkanAccessPermissions;
		using PipelineStages	= VulkanPipelineStages;
		using ImageLayouts		= VulkanImageLayouts;
		using PipelineStages	= VulkanPipelineStages;
		using AttachmentIO		= VulkanAttachmentIO;

		class CommandContext;

	public:
		CALL RegisterCommandContext(const String& Name, PipelineStages::Option Deadline) -> void;
		CALL SearchCommandContext(StringView Name)	-> SharedPtr<CommandContext>;

		CALL CreateFence()						-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(); }
		CALL CreateSignaledFence()				-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(true); }
		CALL CreateSemaphore()					-> SharedPtr<Semaphore> { return CreateSharedPtr<Semaphore>(); }
		CALL CreateSignaledSemaphore()			-> SharedPtr<Semaphore> { Assert(False, "Not Supported by Vulkan"); return CreateSharedPtr<Semaphore>(true); }
		CALL CreateShader(ShaderStages Stage, const Array<Byte>& ShadingCode) -> SharedPtr<Shader> { return CreateSharedPtr<VulkanShader>(Stage, ShadingCode);}

		CALL GetSwapchain() -> const Swapchain& { return Vulkan::Swapchain; }
	public:
		class CommandContext
		{
			friend class RHI;
			friend class Render; //[TODO] Remove
		public:
			StringView						 Name;
			SharedPtr<CommandBuffer>		 Commands;

		private:
			void Create(StringView Name, PipelineStages::Option Deadline, SharedPtr<Fence> SignalFence);
			void Destroy() noexcept {};

		private:
			PipelineStages::Option			 Deadline;
			Array<SharedPtr<CommandContext>> Dependencies;
			Semaphore						 Semaphore_Compeleted;
			SharedPtr<Fence>				 Fence_Compeleted;
		};


	private:
		struct Frame
		{
			Fence		Fence_ReadyToRender{ True };
			Semaphore	Semaphore_ReadyToRender;
			Semaphore	Semaphore_ReadyToPresent;
			HashMap<String, SharedPtr<CommandContext>> CommandContexts;
		};
		static inline Array<Frame> Frames;

		CALL GetCurrentFrame() -> Frame& { return Frames[Vulkan::Swapchain.GetCursor()]; }

		static inline void
		WaitForCurrentFrame() throw(Swapchain::RecreateSignal)
		{
			auto& CurrentFrame = GetCurrentFrame();
			//CurrentFrame.Fence_ReadyToRender.Wait();
			Vulkan::Swapchain.WaitForCurrentImage(CurrentFrame.Semaphore_ReadyToRender);
		}
		static inline void
		PresentCurrentFrame()	throw(Swapchain::RecreateSignal)
		{
			auto& CurrentFrame = GetCurrentFrame();
			Vulkan::Swapchain.PresentCurrentImage(CurrentFrame.Semaphore_ReadyToPresent);
			CurrentFrame.Fence_ReadyToRender.Reset(); //Reset to Signaled
			GetCurrentFrame().Fence_ReadyToRender.Wait();//[FIXME]
		}

	private:
		static inline CommandPool ResetableGraphicsCommandPool{};
		static inline CommandPool TransientGraphicsCommandPool{};

	private:
		RHI() noexcept = default;
		static void
		Bootstrap()
		{
			Vulkan::Bootstrap();
			ResetableGraphicsCommandPool.Create(VulkanDevice::QueueFamilyType::Graphics, VulkanCommandPool::PoolType::Resetable);
			TransientGraphicsCommandPool.Create(VulkanDevice::QueueFamilyType::Graphics, VulkanCommandPool::PoolType::Transient);
			Frames.resize(Vulkan::Swapchain.GetSize());
		}
		static void
		Terminate()
		{
			Frames.clear();
			TransientGraphicsCommandPool.Destroy();
			ResetableGraphicsCommandPool.Destroy();
			Vulkan::Terminate();
		}
	};

	void RHI::CommandContext::
	Create(	StringView				Name,
			PipelineStages::Option	Deadline,
			SharedPtr<Fence>		SignalFence)
	{
		this->Name				= std::move(Name);
		this->Deadline			= Deadline;
		this->Fence_Compeleted	= std::move(SignalFence);
	}

	void RHI::
	RegisterCommandContext(const String& Name, PipelineStages::Option Deadline)
	{
		for (auto& Frame : Frames)
		{
			Assert(!Frame.CommandContexts.count(Name));
			auto& NewCommandContext = Frame.CommandContexts[Name];
			NewCommandContext = CreateSharedPtr<CommandContext>();
			NewCommandContext->Create(Name, Deadline, nullptr);
			//[FIXME]: Create Buffer based on Create(X_para)
			NewCommandContext->Commands = ResetableGraphicsCommandPool.Allocate(CommandBuffer::Level::Primary);
		}
		auto& CurrentFrame = GetCurrentFrame();
	}

	SharedPtr<RHI::CommandContext> RHI::
	SearchCommandContext(StringView Name)
	{
		auto& CurrentFrame = GetCurrentFrame();
		auto Result = CurrentFrame.CommandContexts.find(Name.data());

		if(Result == CurrentFrame.CommandContexts.end())
		{ Log::Fatal("Failed to search Command Context ({})", Name); }

		return Result->second;
	}

} } // namespace VE::Runtime