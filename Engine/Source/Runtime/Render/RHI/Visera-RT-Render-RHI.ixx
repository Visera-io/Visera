module;
#include <Visera.h>
#include "Vulkan/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI;
#define VE_MODULE_NAME "RHI"
import Visera.Runtime.Render.RHI.Vulkan;
export import Visera.Runtime.Render.RHI.Vulkan.Common;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.Time;
import Visera.Core.Math.Basic;
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
		using FFence				= FVulkanFence;
		using FSPIRVShader			= FVulkanSPIRVShader;
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
		using ECullMode             = EVulkanCullMode;

		using SSwapchainRecreation = FVulkanSwapchain::SRecreation;

		enum ESystemRenderTarget : UInt8 { SV_Color, SV_Depth };
		struct alignas(16) FMatrixUBOLayout
		{
			Matrix4x4F Projection     = Matrix4x4F::Identity();
			Matrix4x4F Viewing	      = Matrix4x4F::Identity();
			Matrix4x4F Model          = Matrix4x4F::Identity();

			Matrix4x4F InverseProjection = Matrix4x4F::Identity();
			Matrix4x4F InverseViewing    = Matrix4x4F::Identity();
			Matrix4x4F CofactorModel     = Matrix4x4F::Identity(); //Use Matrix3x3F in Shaders
		};

		class FFrameContext
		{
			friend class RHI;
		public:
			auto GetGraphicsCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer>    { return CommandBuffers[Graphics]; }
			auto GetEditorCommandBuffer()   -> SharedPtr<FGraphicsCommandBuffer>    { return CommandBuffers[Editor];   }

			auto GetColorImageShaderReadView() const -> SharedPtr<const FImageView> { return SVColorView; }
			auto GetSVColorTexture()    const -> SharedPtr<const FDescriptorSet>	{ return SVColorTexture; }
			auto GetFinalColorTexture() const -> SharedPtr<const FDescriptorSet>    { return PostprocessOutputTexture; }

			auto GetMatrixUBO() const -> SharedPtr<const FDescriptorSet> { return MatrixUBO; }

			void SetModelMatrix(const Matrix4x4F& _ModelMatrix)
			{ MatrixData->Write(_ModelMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Model)); }
			void SetViewingMatrix(const Matrix4x4F& _ViewingMatrix)
			{ MatrixData->Write(_ViewingMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Viewing)); }
			void SetProjectionMatrix(const Matrix4x4F& _ProjectionMatrix)
			{ MatrixData->Write(_ProjectionMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Projection)); }

			void SetInverseProjectionMatrix(const Matrix4x4F& _InverseProjectionMatrix)
			{ MatrixData->Write(_InverseProjectionMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, InverseProjection)); }
			void SetInverseViewingMatrix(const Matrix4x4F& _InverseViewingMatrix)
			{ MatrixData->Write(_InverseViewingMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, InverseViewing)); }
			void SetCofactorModelMatrix(const Matrix4x4F& _CofactorModelMatrix)
			{ MatrixData->Write(_CofactorModelMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, CofactorModel)); }

			Bool IsReady() const { return !InFlightFence.IsBlocking(); }

		private:
			static inline FRenderArea RenderArea{ {0,0},{UInt32(Window::GetExtent().Width), UInt32(Window::GetExtent().Height) } }; //[FIXME]: Read from Config
			static inline SharedPtr<FDescriptorSetLayout> SVColorTextureDSLayout;
			static inline SharedPtr<FSampler>			  SVColorTextureSampler;
			static inline SharedPtr<FDescriptorSetLayout> MatrixDSLayout;

			SharedPtr<FRenderTarget> BackgroundRTs;
			SharedPtr<FRenderTarget> ForwardRTs;
			SharedPtr<FRenderTarget> PostprocessingRTs;

			SharedPtr<FImageView>     SVColorView;
			SharedPtr<FDescriptorSet> SVColorTexture;
			SharedPtr<FImageView>     PostprocessInputView;
			SharedPtr<FDescriptorSet> PostprocessInputTexture;
			SharedPtr<FImageView>     PostprocessOutputView;
			SharedPtr<FDescriptorSet> PostprocessOutputTexture;

			SharedPtr<FBuffer>        MatrixData;
			SharedPtr<FDescriptorSet> MatrixUBO;

			enum EFrameCommandBufferType { Resource, Graphics, Editor, MAX };
			Segment<SharedPtr<FGraphicsCommandBuffer>, MAX> CommandBuffers;
			Segment<FSemaphore, MAX>                        CommandsFinishedSemaphores;

			FSemaphore SwapchainReadySemaphore			= RHI::CreateSemaphore();
			FFence	   InFlightFence					= RHI::CreateFence(FFence::EStatus::Signaled);
		};

	public:
		static inline SharedPtr<FDescriptorSetLayout>
		CreateDescriptorSetLayout();
		static inline SharedPtr<FDescriptorSet>
		CreateDescriptorSet(SharedPtr<const FDescriptorSetLayout> _SetLayout);
		
		static inline SharedPtr<FGraphicsCommandBuffer>
		CreateGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary);
		static inline SharedPtr<FGraphicsCommandBuffer>
		CreateOneTimeGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary);

		static inline SharedPtr<FImage>
		CreateImage(EImageType   _Type,
			        FExtent3D    _Extent,
			        EFormat      _Format,
			        EImageAspect _Aspects,
			        EImageUsage  _Usages,
			        EImageTiling _Tiling       = EImageTiling::Optimal,
			        ESampleRate  _SampleRate   = ESampleRate::X1,
			        UInt8        _MipmapLevels = 1,
			        UInt8        _ArrayLayers  = 1,
			        ESharingMode _SharingMode  = ESharingMode::Exclusive,
			        EMemoryUsage _Location     = EMemoryUsage::Auto);
		static inline SharedPtr<FSampler>
		CreateSampler(EFilter _Filter, ESamplerAddressMode _AddressMode = ESamplerAddressMode::ClampToEdge);

		static inline SharedPtr<FBuffer>
		CreateBuffer(UInt64 _Size,
			         EBufferUsage _Usages,
			         ESharingMode _SharingMode     = EVulkanSharingMode::Exclusive,
			         EMemoryUsage _Location        = EMemoryUsage::Auto,
			         UInt32       _AllocationFlags = 0x0);
		static inline SharedPtr<RHI::FBuffer>
		CreateVertexBuffer(UInt64 _Size);
		static inline SharedPtr<RHI::FBuffer>
		CreateIndexBuffer(UInt64 _Size);
		static inline SharedPtr<RHI::FBuffer>
		CreateStagingBuffer(UInt64 _Size);
		static inline SharedPtr<RHI::FBuffer>
		CreateMappedBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive);

		static inline FFence
		CreateFence(FFence::EStatus _Status = FFence::EStatus::Blocking);
		static inline FSemaphore
		CreateSemaphore();
		static inline SharedPtr<FSPIRVShader>
		CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize);
		template<TRenderPass T> static auto
		CreateRenderPass() -> SharedPtr<T>;
		static inline SharedPtr<RHI::FPipelineLayout>
		CreatePipelineLayout();
		static inline SharedPtr<RHI::FRenderPipelineSetting>
		CreateRenderPipelineSetting();

		static inline FFrameContext&
		WaitFrameReady();
		static inline const Array<FFrameContext>&
		GetFrames() { return Frames;  }
		static inline FFrameContext&
		GetCurrentFrame();
		static inline void
		RenderAndPresentCurrentFrame();
		static inline UInt32
		GetFPS() { return FPS; }

		static inline UInt32
		GetSwapchainFrameCount() { return Vulkan->GetSwapchain().GetFrameCount(); }
		static inline UInt32
		GetSwapchainCursor()     { return Vulkan->GetSwapchain().GetCursor();     }
		static inline EFormat
		GetSwapchainFormat()     { return Vulkan->GetSwapchain().GetFormat();     }

		static inline void
		WaitDeviceIdle()		 { Vulkan->GetDevice().WaitIdle(); }
		static inline FVulkan*
		GetAPI()				 { return Vulkan; }
		static inline SharedPtr<const FDescriptorPool>
		GetGlobalDescriptorPool() { return GlobalDescriptorPool; }

		static inline Bool
		IsTexture2DFormatSupported(EFormat _Format) { return Vulkan->GetGPU().IsTexture2DFormatSupported(_Format); }

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
			VE_LOG_TRACE("Initializing Vulkan...");
			Vulkan = new FVulkan();
			VE_LOG_TRACE("Creating Global Descriptor Pool...");
			GlobalDescriptorPool = FDescriptorPool::Create()
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
				->AddEntry(EDescriptorType::Sampler,              1000)
				->Build(10000);
			ResetableGraphicsCommandPool = FGraphicsCommandPool::Create(ECommandPoolType::Resetable);
			TransientGraphicsCommandPool = FGraphicsCommandPool::Create(ECommandPoolType::Transient);

			VE_LOG_TRACE("Creating Frames...");
			Frames.resize(GetSwapchainFrameCount());
			auto ImmeCmds = CreateOneTimeGraphicsCommandBuffer();
			ImmeCmds->StartRecording();

			FFrameContext::SVColorTextureDSLayout = RHI::CreateDescriptorSetLayout()
				->AddBinding(0, EDescriptorType::CombinedImageSampler, 1, EShaderStage::Fragment)
				->Build();

			FFrameContext::SVColorTextureSampler = RHI::CreateSampler(EFilter::Linear)
				->Build();

			FFrameContext::MatrixDSLayout = RHI::CreateDescriptorSetLayout()
				->AddBinding(0, EDescriptorType::UniformBuffer, 1, EShaderStage::Vertex)
				->Build();

			for (auto& Frame : Frames)
			{
				for (auto& CommandBuffer : Frame.CommandBuffers)
				{
					CommandBuffer = RHI::CreateGraphicsCommandBuffer();
				}
				for (auto& Semephore : Frame.CommandsFinishedSemaphores)
				{
					Semephore = RHI::CreateSemaphore();
				}

				Frame.MatrixData = CreateMappedBuffer(sizeof(FMatrixUBOLayout), EBufferUsage::Uniform);
				Frame.MatrixUBO = RHI::CreateDescriptorSet(FFrameContext::MatrixDSLayout);
				Frame.MatrixUBO->WriteBuffer(0, Frame.MatrixData, 0, Frame.MatrixData->GetSize());

				auto ColorImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferSource,
					EImageTiling::Optimal,
					ESampleRate::X1
				);
				auto PostprocessImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferSource,
					EImageTiling::Optimal,
					ESampleRate::X1
				);
				ImmeCmds->ConvertImageLayout(ColorImage, EVulkanImageLayout::ShaderReadOnly);
				ImmeCmds->ConvertImageLayout(PostprocessImage, EVulkanImageLayout::ShaderReadOnly);

				Frame.SVColorView = ColorImage->CreateImageView();
				Frame.SVColorTexture = RHI::CreateDescriptorSet(FFrameContext::SVColorTextureDSLayout);
				Frame.SVColorTexture->WriteImage(0, Frame.SVColorView, FFrameContext::SVColorTextureSampler);

				Frame.PostprocessInputView     = Frame.SVColorView;
				Frame.PostprocessInputTexture  = Frame.SVColorTexture;
				Frame.PostprocessOutputView    = PostprocessImage->CreateImageView();
				Frame.PostprocessOutputTexture = RHI::CreateDescriptorSet(FFrameContext::SVColorTextureDSLayout);
				Frame.PostprocessOutputTexture->WriteImage(0, Frame.PostprocessOutputView, FFrameContext::SVColorTextureSampler);

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

				Frame.BackgroundRTs = FRenderTarget::Create()
					->AddColorImage(ColorImage)
					->AddDepthImage(DepthImage)
				    ->Confirm();

				Frame.ForwardRTs = FRenderTarget::Create()
					->AddColorImage(ColorImage)
					->AddDepthImage(DepthImage)
				    ->Confirm();

				Frame.PostprocessingRTs = FRenderTarget::Create()
					->AddColorImage(PostprocessImage)
				    ->Confirm();
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

			EGraphicsPipelineStage WaitStages = EGraphicsPipelineStage::PipelineTop;
			ImmeCmds->Submit(
				FCommandSubmitInfo
				{
					.WaitSemaphoreCount		= 0,
					.pWaitSemaphores		= nullptr,
					.pWaitStages			{.Graphics = &WaitStages },
					.SignalSemaphoreCount	= 0,
					.pSignalSemaphores		= nullptr,
					.SignalFence			= &Fence,
				});
			Fence.Wait();
		}

		static void inline
		Terminate()
		{
			VE_LOG_TRACE("Terminating RHI...");
			WaitDeviceIdle();
			VE_LOG_TRACE("Destroying Frames...");
			Frames.clear();
			FFrameContext::SVColorTextureDSLayout->Destroy();
			FFrameContext::SVColorTextureSampler->Destroy();
			FFrameContext::MatrixDSLayout->Destroy();
			VE_LOG_TRACE("Destroying Command Pools...");
			TransientGraphicsCommandPool.reset();
			ResetableGraphicsCommandPool.reset();
			VE_LOG_TRACE("Destroying Global Descriptor Pool...");
			GlobalDescriptorPool->Destroy();
			VE_LOG_TRACE("Finalizing Vulkan...");
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
		VE_LOG_DEBUG("Creating a new render pass ({})", typeid(T).name());
		auto NewRenderPass = CreateSharedPtr<T>();
		Array<SharedPtr<FRenderTarget>> RenderTargets;

		switch (FRenderPass::EType(NewRenderPass->GetType()))
		{
		case FRenderPass::EType::Background:
		{
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].BackgroundRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::DefaultForward:
		{
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].ForwardRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::Postprocessing:
		{
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].PostprocessingRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
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
			NewRenderPass->Build(SwapchainArea, {/*Handle inside the Renderpass*/});
			break;
		}
		case FRenderPass::EType::Customized:
			throw SRuntimeError("Customized Render Pass WIP...");
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

			auto& EditorCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Editor];
			EditorCommandBuffer->SetStatus(FGraphicsCommandBuffer::EStatus::Idle);
			EditorCommandBuffer->Reset();
			//Start Recording at Editor

			auto& GraphicsCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Graphics];
			GraphicsCommandBuffer->SetStatus(FGraphicsCommandBuffer::EStatus::Idle);
			GraphicsCommandBuffer->Reset();
			GraphicsCommandBuffer->StartRecording();

			auto& ResourceCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Resource];
			ResourceCommandBuffer->SetStatus(FGraphicsCommandBuffer::EStatus::Idle);
			ResourceCommandBuffer->Reset();
			ResourceCommandBuffer->StartRecording();
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

		auto& ResourceCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Resource];
		ResourceCommandBuffer->StopRecording();
		static Segment<EVulkanGraphicsPipelineStage, 1> ResourceWaitStages
		{
			EVulkanGraphicsPipelineStage::PipelineTop
		};
		ResourceCommandBuffer->Submit(
			FCommandSubmitInfo
			{
				.WaitSemaphoreCount		= 0,
				.pWaitSemaphores		= nullptr,
				.pWaitStages			{.Graphics = ResourceWaitStages.data()},
				.SignalSemaphoreCount	= 1,
				.pSignalSemaphores		= (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Resource]),
				.SignalFence			= nullptr,
			});

		auto& GraphicsCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Graphics];
		GraphicsCommandBuffer->StopRecording();

		static Segment<VkSemaphore, 2> GraphicsWaitSemaphores;
		GraphicsWaitSemaphores[0] = CurrentFrame.SwapchainReadySemaphore.GetHandle();
		GraphicsWaitSemaphores[1] = CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Resource].GetHandle();
		static Segment<EVulkanGraphicsPipelineStage, 2> GraphicsWaitStages
		{
			EVulkanGraphicsPipelineStage::PipelineTop,
			EVulkanGraphicsPipelineStage::PipelineTop,
		};

		GraphicsCommandBuffer->Submit(
			FCommandSubmitInfo
			{
				.WaitSemaphoreCount		= UInt32(GraphicsWaitSemaphores.size()),
				.pWaitSemaphores		= GraphicsWaitSemaphores.data(),
				.pWaitStages            {.Graphics = GraphicsWaitStages.data()},
				.SignalSemaphoreCount	= 1,
				.pSignalSemaphores		= (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Graphics]),
				.SignalFence			= nullptr,
			});

		auto& EditorCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Editor];
		VE_ASSERT(FCommandBuffer::EStatus::ReadyToSubmit == EditorCommandBuffer->GetStatus()) //Stop Recording at Editor
		static Segment<EVulkanGraphicsPipelineStage, 1> EditorWaitStages
		{
			EVulkanGraphicsPipelineStage::PipelineTop
		};
		EditorCommandBuffer->Submit(
			FCommandSubmitInfo
			{
				.WaitSemaphoreCount		= 1,
				.pWaitSemaphores		= (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Graphics]),
				.pWaitStages			{.Graphics = EditorWaitStages.data()},
				.SignalSemaphoreCount	= 1,
				.pSignalSemaphores		= (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Editor]),
				.SignalFence			= &CurrentFrame.InFlightFence,
			});

		try
		{
			Vulkan->GetSwapchain().Present(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Editor], 1);
		}
		catch (const SSwapchainRecreation& Signal)
		{
			VE_LOG_DEBUG("Recreating swapchain...");
			throw SRuntimeError("WIP: Swapchain Recreation!");
		}
	}

	SharedPtr<RHI::FDescriptorSetLayout> RHI::
	CreateDescriptorSetLayout()
	{ 
		return FDescriptorSetLayout::Create();
	}

	SharedPtr<RHI::FDescriptorSet> RHI::
	CreateDescriptorSet(SharedPtr<const FDescriptorSetLayout> _SetLayout)
	{
		VE_LOG_DEBUG("Creating a new descriptor set (layout:{}).", (Address)(_SetLayout->GetHandle()));
		return GlobalDescriptorPool->CreateDescriptorSet(_SetLayout);
	}

	SharedPtr<RHI::FSampler> RHI::
	CreateSampler(EFilter _Filter, ESamplerAddressMode _AddressMode/* = ESamplerAddressMode::ClampToEdge*/)
	{
		return FSampler::Create(_Filter, _AddressMode);
	}

	SharedPtr<RHI::FGraphicsCommandBuffer> RHI::
	CreateGraphicsCommandBuffer(ECommandLevel _Level/* = ECommandLevel::Primary*/)
	{
		return ResetableGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level);
	}

	SharedPtr<RHI::FGraphicsCommandBuffer> RHI::
	CreateOneTimeGraphicsCommandBuffer(ECommandLevel _Level/* = ECommandLevel::Primary*/)
	{ 
		return TransientGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level);
	}

	SharedPtr<RHI::FImage> RHI::
	CreateImage(EImageType   _Type,
		        FExtent3D    _Extent,
		        EFormat      _Format,
		        EImageAspect _Aspects,
		        EImageUsage  _Usages,
		        EImageTiling _Tiling      /* = EImageTiling::Optimal*/,
		        ESampleRate  _SampleRate  /* = ESampleRate::X1*/,
		        UInt8        _MipmapLevels/* = 1*/,
		        UInt8        _ArrayLayers /* = 1*/,
		        ESharingMode _SharingMode /* = ESharingMode::Exclusive*/,
		        EMemoryUsage _Location    /* = EMemoryUsage::Auto*/)
	{
		VE_LOG_DEBUG("Creating a new image (format:{}, extent:[{}, {}, {}])",
			UInt32(_Format), _Extent.width, _Extent.height, _Extent.depth);

		return Vulkan->GetAllocator().CreateImage(
			_Type, _Extent, _Format, _Aspects, _Usages,
			_Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location);
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateBuffer(UInt64       _Size,
		         EBufferUsage _Usages,
		         ESharingMode _SharingMode     /*= EVulkanSharingMode::Exclusive*/,
		         EMemoryUsage _Location        /*= EMemoryUsage::Auto*/,
		         UInt32       _AllocationFlags /*= 0x0*/)
	{
		VE_LOG_DEBUG("Creating a new buffer (size:{})", _Size);
		return Vulkan->GetAllocator().CreateBuffer(_Size, _Usages, _SharingMode, _Location, _AllocationFlags);
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateMappedBuffer(UInt64 _Size,
		EBufferUsage _Usages,
		ESharingMode _SharingMode/* = EVulkanSharingMode::Exclusive*/)
	{
		return CreateBuffer(
			_Size,
			_Usages,
			_SharingMode,
			EMemoryUsage::CPU,
			VMA_ALLOCATION_CREATE_MAPPED_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateStagingBuffer(UInt64 _Size)
	{ 
		return CreateBuffer(
			_Size,
			EBufferUsage::TransferSource,
			EVulkanSharingMode::Exclusive,
			EMemoryUsage::Auto,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateVertexBuffer(UInt64 _Size)
	{
		return CreateBuffer(_Size, EBufferUsage::Vertex | EBufferUsage::TransferDestination);
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateIndexBuffer(UInt64 _Size) 
	{ 
		return CreateBuffer(_Size, EBufferUsage::Index | EBufferUsage::TransferDestination);
	}

	RHI::FFence RHI::
	CreateFence(FFence::EStatus _Status/* = FFence::EStatus::Blocking*/)
	{
		return FFence{_Status};
	}

	RHI::FSemaphore RHI::
	CreateSemaphore()
	{
		return FSemaphore();
	}

	SharedPtr<RHI::FSPIRVShader> RHI::
	CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize)
	{
		VE_LOG_DEBUG("Creating a new shader (stage:{}, size: {})", UInt32(_ShaderStage), _CodeSize);
		return FSPIRVShader::Create(_ShaderStage, _SPIRVCode, _CodeSize);
	}

	SharedPtr<RHI::FPipelineLayout> RHI::
	CreatePipelineLayout()
	{
		return FPipelineLayout::Create();
	}

	SharedPtr<RHI::FRenderPipelineSetting> RHI::
	CreateRenderPipelineSetting()
	{
		return FRenderPipelineSetting::Create();
	}

} // namespace VE