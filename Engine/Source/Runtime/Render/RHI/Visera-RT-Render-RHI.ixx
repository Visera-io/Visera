module;
#include <Visera.h>
#include "Vulkan/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.RHI.Vulkan;
export import Visera.Runtime.Render.RHI.Vulkan.Common;

import Visera.Core.Signal;
import Visera.Core.Time;
import Visera.Runtime.Platform.Window;

export namespace VE
{
	template<typename T>
	concept TRenderPass = std::is_base_of_v<FVulkanRenderPass, T>;

	/* This is not a thread-safe RHI! */
	class RHI
	{
		VE_MODULE_MANAGER_CLASS(RHI);
	public:
		using FSemaphore			= FVulkanSemaphore;
		using FGraphicsCommandPool	= FVulkanGraphicsCommandPool;
		using FGraphicsCommandBuffer= FVulkanGraphicsCommandBuffer;
		using FDescriptorPool		= FVulkanDescriptorPool;
		using FDescriptorSet		= FVulkanDescriptorSet;
		using FDescriptorSetLayout  = FVulkanDescriptorSetLayout;
		using FDescriptorBinding	= FVulkanDescriptorSetLayout::FBinding;
		using FFence				= FVulkanFence;
		using FShader				= FVulkanShader;
		using FBuffer				= FVulkanBuffer;
		using FImage				= FVulkanImage;
		using FImageView			= FVulkanImageView;
		using FImageMemoryBarrier   = FVulkanImageMemoryBarrier;
		using FExtent2D				= FVulkanExtent2D;
		using FExtent3D				= FVulkanExtent3D;
		using FOffset2D				= FVulkanOffset2D;
		using FOffset3D				= FVulkanOffset3D;
		using FFramebuffer			= FVulkanFramebuffer;
		using FRenderPass			= FVulkanRenderPass;
		using FRenderPassLayout		= FVulkanRenderPassLayout;
		using FRenderTarget			= FVulkanRenderTarget;
		using FRenderPipeline		= FVulkanRenderPipeline;
		using FPipelineLayout		= FVulkanPipelineLayout;
		using FRenderPipelineSetting= FVulkanRenderPipelineSetting;
		using FPushConstantRange	= FVulkanPipelineLayout::FPushConstantRange;
		using FCommandPool          = FVulkanCommandPool;
		using FCommandBuffer        = FVulkanCommandBuffer;
		using FCommandSubmitInfo	= FVulkanCommandSubmitInfo;
		using FRenderArea			= FVulkanRenderArea;
		using FSampler				= FVulkanSampler;

		using ESharingMode			= EVulkanSharingMode;
		using ESampleRate			= EVulkanSampleRate;
		using EQueueFamily			= EVulkanQueueFamily;
		using ECommandPoolType		= EVulkanCommandPoolType;
		using ECommandLevel			= EVulkanCommandLevel;
		using EShaderStage			= EVulkanShaderStage;
		using EAccess				= EVulkanAccess;
		using EGraphicsPipelineStage= EVulkanGraphicsPipelineStage;
		using EComputePipelineStage = EVulkanComputePipelineStage;
		using ETransferPipelineStage= EVulkanTransferPipelineStage;
		using EMemoryUsage			= EVulkanMemoryUsage;
		using EBufferUsage			= EVulkanBufferUsage;
		using EAttachmentIO			= EVulkanAttachmentIO;
		using EDescriptorType		= EVulkanDescriptorType;
		using EImageType			= EVulkanImageType;
		using EImageLayout			= EVulkanImageLayout;
		using EImageUsage			= EVulkanImageUsage;
		using EImageViewType		= EVulkanImageViewType;
		using EImageAspect			= EVulkanImageAspect;
		using EImageTiling			= EVulkanImageTiling;
		using EFormat				= EVulkanFormat;
		using EFilter				= EVulkanFilter;
		using EPresentMode			= EVulkanPresentMode;
		using EBorderColor			= EVulkanBorderColor;
		using ESamplerMipmapMode	= EVulkanSamplerMipmapMode;
		using ESamplerAddressMode	= EVulkanSamplerAddressMode;

		using SSwapchainRecreation = FVulkanSwapchain::SRecreation;

		enum ESystemRenderTarget : UInt8 { SV_Color, SV_Depth };
		class FFrameContext
		{
			friend class RHI;
		public:
			auto GetGraphicsCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer>    { return GraphicsCommandBuffer; }
			auto GetEditorCommandBuffer()   -> SharedPtr<FGraphicsCommandBuffer>    { return EditorCommandBuffer;   }
			auto GetColorImageShaderReadView() const -> SharedPtr<const FImageView> { return ColorImageShaderReadView; }

			Bool IsReady() const { return !InFlightFence.IsBlocking(); }

		private:
			static inline FRenderArea RenderArea{ {0,0},{UInt32(Window::GetExtent().Width), UInt32(Window::GetExtent().Height) } }; //[FIXME]: Read from Config
			SharedPtr<FRenderTarget> RenderTarget = CreateSharedPtr<FRenderTarget>();
			SharedPtr<FImageView>    ColorImageShaderReadView;

			SharedPtr<FGraphicsCommandBuffer> GraphicsCommandBuffer	= RHI::CreateGraphicsCommandBuffer();
			SharedPtr<FGraphicsCommandBuffer> EditorCommandBuffer	= RHI::CreateGraphicsCommandBuffer();
			FSemaphore GraphicsCommandsFinishedSemaphore= RHI::CreateSemaphore();
			FSemaphore EditorCommandsFinishedSemaphore	= RHI::CreateSemaphore();

			FSemaphore SwapchainReadySemaphore			= RHI::CreateSemaphore();
			FFence	   InFlightFence					= RHI::CreateFence(FFence::EStatus::Signaled);
		};

	public:
		static inline auto
		CreateDescriptorSetLayout(const Array<FDescriptorBinding> _Bindings)		-> SharedPtr<FDescriptorSetLayout>	{ return CreateSharedPtr<FDescriptorSetLayout>(_Bindings); }
		static inline auto
		CreateDescriptorSet(SharedPtr<const FDescriptorSetLayout> _SetLayout)		-> SharedPtr<FDescriptorSet>		{ return GlobalDescriptorPool->CreateDescriptorSet(_SetLayout);		}
		static inline auto
		CreateSampler(EFilter _Filter, ESamplerAddressMode _AddressMode = ESamplerAddressMode::ClampToEdge)																-> SharedPtr<FSampler>				{ return CreateSharedPtr<FSampler>(_Filter, _AddressMode); }
		static inline auto
		CreateGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return ResetableGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		static inline auto
		CreateOneTimeGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return TransientGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		static inline auto
		CreateImage(EImageType _Type, FExtent3D _Extent, EFormat _Format, EImageAspect _Aspects, EImageUsage _Usages, EImageTiling _Tiling = EImageTiling::Optimal,ESampleRate _SampleRate = ESampleRate::X1, UInt8 _MipmapLevels = 1,UInt8 _ArrayLayers = 1, ESharingMode	_SharingMode = ESharingMode::Exclusive,EMemoryUsage	_Location = EMemoryUsage::Auto)->SharedPtr<FImage> { return Vulkan->GetAllocator().CreateImage(_Type, _Extent, _Format, _Aspects, _Usages, _Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location); }
		static inline auto
		CreateBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer> { return Vulkan->GetAllocator().CreateBuffer(_Size, _Usages, _SharingMode, _Location); }
		static inline auto
		CreateVertexBuffer(UInt64 _Size) { return CreateBuffer(_Size, EBufferUsage::Vertex | EBufferUsage::TransferDestination); }
		static inline auto
		CreateIndexBuffer(UInt64 _Size)  { return CreateBuffer(_Size, EBufferUsage::Index | EBufferUsage::TransferDestination);  }
		static inline auto
		CreateStagingBuffer(UInt64 _Size, EBufferUsage _Usages = EBufferUsage::TransferSource, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer>;
		static inline auto
		CreateFence(FFence::EStatus _Status = FFence::EStatus::Blocking)			-> FFence		 { return FFence{_Status};	}
		static inline auto
		CreateSemaphore()															-> FSemaphore	 { return FSemaphore();	}
		static inline auto
		CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize) -> SharedPtr<FShader> { return CreateSharedPtr<FShader>(_ShaderStage, _SPIRVCode, _CodeSize); }
		template<TRenderPass T> static auto
		CreateRenderPass() -> SharedPtr<T>;
		static inline auto
		CreatePipelineLayout() { return FPipelineLayout::Create(); }
		static inline auto
		CreateRenderPipelineSetting() { return FRenderPipelineSetting::Create(); }

		static inline auto
		WaitFrameReady() -> FFrameContext&;
		static inline auto
		GetFrames() -> const Array<FFrameContext>& { return Frames;  }
		static inline auto
		GetCurrentFrame() -> FFrameContext&;
		static inline auto
		RenderAndPresentCurrentFrame() -> void;
		static inline auto
		GetFPS() -> UInt32 { return FPS; }

		static inline auto
		GetSwapchainFrameCount()	-> UInt32		  { return Vulkan->GetSwapchain().GetFrameCount(); }
		static inline auto
		GetSwapchainCursor()		-> UInt32		  { return Vulkan->GetSwapchain().GetCursor(); }
		static inline auto
		GetSwapchainFormat()		-> EFormat		  { return Vulkan->GetSwapchain().GetFormat(); }

		static inline auto
		WaitDeviceIdle()		 -> void { Vulkan->GetDevice().WaitIdle(); }
		static inline auto
		GetAPI()				 -> FVulkan* { return Vulkan; }
		static inline auto
		GetGlobalDescriptorPool() -> SharedPtr<const FDescriptorPool> { return GlobalDescriptorPool; }

	private:
		static inline FVulkan*							Vulkan;
		//[TODO]: ThreadSafe Paradigm
		static inline SharedPtr<FDescriptorPool>		GlobalDescriptorPool{};
		static inline SharedPtr<FGraphicsCommandPool>	ResetableGraphicsCommandPool{};
		static inline SharedPtr<FGraphicsCommandPool>	TransientGraphicsCommandPool{};

		static inline Array<FFrameContext> Frames;
		static inline UInt32 FPS{ 0 };

	public:
		static void inline
		Bootstrap()
		{
			Vulkan = new FVulkan();
			GlobalDescriptorPool = FDescriptorPool::Create();
			GlobalDescriptorPool
				->AddEntry(EDescriptorType::UniformBuffer,        1000)
				->AddEntry(EDescriptorType::StorageBuffer,        1000)
				->AddEntry(EDescriptorType::DynamicStorageBuffer, 1000)
				->AddEntry(EDescriptorType::DynamicUniformBuffer, 1000)
				->AddEntry(EDescriptorType::CombinedImageSampler, 1000)
				->AddEntry(EDescriptorType::InputAttachment,      1000)
				->AddEntry(EDescriptorType::SampledImage,         1000)
				->AddEntry(EDescriptorType::StorageImage,         1000)
				->AddEntry(EDescriptorType::StorageTexelBuffer,   1000)
				->AddEntry(EDescriptorType::UniformTexelBuffer,   1000)
				->AddEntry(EDescriptorType::Sampler, 1000)
				->Build(10000);
			ResetableGraphicsCommandPool = FGraphicsCommandPool::Create(ECommandPoolType::Resetable);
			TransientGraphicsCommandPool = FGraphicsCommandPool::Create(ECommandPoolType::Transient);

			Frames.resize(GetSwapchainFrameCount());

			auto ImmeCmds = CreateOneTimeGraphicsCommandBuffer();
			ImmeCmds->StartRecording();

			for (auto& Frame : Frames)
			{
				auto ColorImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferSource,
					EImageTiling::Optimal,
					ESampleRate::X1
				);
				ImmeCmds->ConvertImageLayout(ColorImage, EVulkanImageLayout::ShaderReadOnly);
				Frame.RenderTarget->AddColorImage(ColorImage);
				Frame.ColorImageShaderReadView = ColorImage->CreateImageView();

				auto DepthImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::S32_Float_Depth32,
					EImageAspect::Depth,
					EImageUsage::DepthStencilAttachment,
					EImageTiling::Optimal,
					ESampleRate::X1
				);
				ImmeCmds->ConvertImageLayout(DepthImage, EVulkanImageLayout::DepthStencilAttachment);
				Frame.RenderTarget->AddDepthImage(DepthImage);
			}

			//[TODO]: Move to VulkanSwapchain Class?
			for (auto& SwapchainImage : Vulkan->GetSwapchain().GetImages())
			{
				VkImageMemoryBarrier SwapchainTransferBarrier
				{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.srcAccessMask = AutoCast(EAccess::None),
					.dstAccessMask = AutoCast(EAccess::None),
					.oldLayout = AutoCast(EImageLayout::Undefined),
					.newLayout = AutoCast(EImageLayout::Present),
					.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.image = SwapchainImage,
					.subresourceRange
					{
						.aspectMask		= AutoCast(EImageAspect::Color),
						.baseMipLevel	= 0,
						.levelCount		= 1,
						.baseArrayLayer = 0,
						.layerCount		= 1
					}
				};
				vkCmdPipelineBarrier(
					ImmeCmds->GetHandle(),
					AutoCast(EGraphicsPipelineStage::PipelineTop),
					AutoCast(EGraphicsPipelineStage::PipelineBottom),
					0x0,		// Dependency Flags
					0, nullptr,	// Memory Barrier
					0, nullptr,	// Buffer Memory Barrier
					1,
					&SwapchainTransferBarrier);
			}
			ImmeCmds->StopRecording();
			auto Fence = CreateFence();

			ImmeCmds->Submit(
				FCommandSubmitInfo
				{
					.WaitSemaphoreCount		= 0,
					.pWaitSemaphores		= nullptr,
					.WaitStages				= {EGraphicsPipelineStage::PipelineTop},
					.SignalSemaphoreCount	= 0,
					.pSignalSemaphores		= nullptr,
					.SignalFence			= &Fence,
				});

			Fence.Wait();
		}

		static void inline
		Terminate()
		{
			WaitDeviceIdle();
			Frames.clear();
			TransientGraphicsCommandPool.reset();
			ResetableGraphicsCommandPool.reset();
			GlobalDescriptorPool->Destroy();
			delete Vulkan;
		}
	};

	RHI::FFrameContext& RHI::
	GetCurrentFrame()
	{
		return Frames[GetSwapchainCursor()];
	}

	template<TRenderPass T>
	SharedPtr<T> RHI::
	CreateRenderPass()
	{
		auto NewRenderPass = CreateSharedPtr<T>();
		Array<SharedPtr<FRenderTarget>> RenderTargets;

		switch (FRenderPass::EType(NewRenderPass->GetType()))
		{
		case FRenderPass::EType::Background:
		case FRenderPass::EType::Customized:
		case FRenderPass::EType::DefaultForward:
		{
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].RenderTarget; }

			NewRenderPass->Create(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::Overlay:
		{
			auto SwapchainExtent = Vulkan->GetSwapchain().GetExtent();
			FRenderArea SwapchainArea
			{
				.offset = {0,0},
				.extent = {SwapchainExtent.width, SwapchainExtent.height},
			};
			NewRenderPass->Create(SwapchainArea, {/*Handle inside the Renderpass*/});
			break;
		}
		default:
			throw SRuntimeError("Failed to create the renderpass -- Unknown RenderPass Type!");
		}

		return NewRenderPass;
	}

	RHI::FFrameContext& RHI::
	WaitFrameReady()
	{
		auto& CurrentFrame = GetCurrentFrame();
		CurrentFrame.InFlightFence.Wait();

		static FSystemClock FPSTimer{};
		static UInt32 FrameTimer{0}, FrameCounter{0};
		FrameTimer		+= FPSTimer.Elapsed().microseconds();
		FrameCounter	+= 1;
		if (FrameTimer >= 1000000) // 1s
		{ FPS = FrameCounter / (FrameTimer / 1000000.0); FPSTimer.Reset(); FrameTimer = 0; FrameCounter = 0; }

		try
		{
			Vulkan->GetSwapchain().WaitForNextImage(CurrentFrame.SwapchainReadySemaphore);
			CurrentFrame.InFlightFence.Block();

			CurrentFrame.EditorCommandBuffer->SetStatus(FGraphicsCommandBuffer::EStatus::Idle);
			CurrentFrame.EditorCommandBuffer->Reset();
			//Start Recording at Editor

			CurrentFrame.GraphicsCommandBuffer->SetStatus(FGraphicsCommandBuffer::EStatus::Idle);
			CurrentFrame.GraphicsCommandBuffer->Reset();
			CurrentFrame.GraphicsCommandBuffer->StartRecording();	
		}
		catch (const SSwapchainRecreation& Signal)
		{
			throw SRuntimeError("WIP: Swapchain Recreation!");
		}

		return CurrentFrame;
	}

	void RHI::
	RenderAndPresentCurrentFrame()
	{
		auto& CurrentFrame = GetCurrentFrame();

		CurrentFrame.GraphicsCommandBuffer->StopRecording();
		CurrentFrame.GraphicsCommandBuffer->Submit(
			FCommandSubmitInfo
			{
				.WaitSemaphoreCount		= 1,
				.pWaitSemaphores		= &CurrentFrame.SwapchainReadySemaphore,//[FIXME]: Temp!
				.WaitStages				= {EGraphicsPipelineStage::PipelineTop},
				.SignalSemaphoreCount	= 1,
				.pSignalSemaphores		= &CurrentFrame.GraphicsCommandsFinishedSemaphore,
				.SignalFence			= nullptr,
			});

		VE_ASSERT(FCommandBuffer::EStatus::ReadyToSubmit == CurrentFrame.EditorCommandBuffer->GetStatus()) //Stop Recording at Editor	
		CurrentFrame.EditorCommandBuffer->Submit(
			FCommandSubmitInfo
			{
				.WaitSemaphoreCount		= 1,
				.pWaitSemaphores		= &CurrentFrame.GraphicsCommandsFinishedSemaphore,
				.WaitStages				= {EGraphicsPipelineStage::FragmentShader},
				.SignalSemaphoreCount	= 1,
				.pSignalSemaphores		= &CurrentFrame.EditorCommandsFinishedSemaphore,
				.SignalFence			= &CurrentFrame.InFlightFence,
			});

		try
		{
			Vulkan->GetSwapchain().Present(&CurrentFrame.EditorCommandsFinishedSemaphore, 1);
		}
		catch (const SSwapchainRecreation& Signal)
		{
			throw SRuntimeError("WIP: Swapchain Recreation!");
		}
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateStagingBuffer(UInt64 _Size,
		EBufferUsage _Usages     /* = EBufferUsage::TransferSource*/,
		ESharingMode _SharingMode/* = EVulkanSharingMode::Exclusive*/,
		EMemoryUsage _Location   /* = EMemoryUsage::Auto*/)
	{ 
		return Vulkan->GetAllocator().CreateBuffer(
			_Size, _Usages, _SharingMode, _Location, 
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

} // namespace VE