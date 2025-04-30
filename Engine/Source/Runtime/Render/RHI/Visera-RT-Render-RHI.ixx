module;
#include <Visera.h>
#include "Vulkan/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI;
#define VE_MODULE_NAME "RHI"
import Visera.Runtime.Render.RHI.Vulkan;
export import Visera.Runtime.Render.RHI.Vulkan.Common;

import Visera.Core.Log;
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
		using FRenderArea			= FVulkanRenderArea;
		using FSampler				= FVulkanSampler;
		using FSwizzle              = FVulkanSwizzle;

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
		using ESwizzle              = EVulkanSwizzle;
		using ECompareMode          = EVulkanCompareOp;

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

		struct alignas(16) FLightUBOLayout
		{
			Matrix4x4F LightSpaceTransform = Matrix4x4F::Identity();
		};

		class FFrameContext
		{
			friend class RHI;
		public:
			//[FIXME]: Read from Config
			static inline FRenderArea RenderArea{ {0,0},{UInt32(Window::GetExtent().Width), UInt32(Window::GetExtent().Height) } };

			auto GetResourceCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer>    { return CommandBuffers[Resource]; }
			auto GetGraphicsCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer>    { return CommandBuffers[Graphics]; }
			auto GetEditorCommandBuffer()   -> SharedPtr<FGraphicsCommandBuffer>    { return CommandBuffers[Editor];   }

			auto GetColorImageShaderReadView() const -> SharedPtr<const FImageView> { return SVColorView; }
			auto GetSVColorTexture()    const -> SharedPtr<const FDescriptorSet>	{ return SVColorTexture; }
			auto GetSVShadowMap()       const -> SharedPtr<const FDescriptorSet>	{ return SVShadowMapTexture; }
			auto GetSVShadowImage()     const -> SharedPtr<FImage>	{ return SVShadowMapImage; }
			auto GetFinalColorTexture() const -> SharedPtr<const FDescriptorSet>    { return PostprocessOutputTexture; }

			auto GetMatrixUBO() const -> SharedPtr<const FDescriptorSet> { return MatrixUBO; }
			auto GetLightUBO()  const -> SharedPtr<const FDescriptorSet> { return LightUBO; }

			void SetModelMatrix(const Matrix4x4F& _ModelMatrix)
			{ MatrixUBOData->Write(_ModelMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Model)); }
			void SetViewingMatrix(const Matrix4x4F& _ViewingMatrix)
			{ MatrixUBOData->Write(_ViewingMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Viewing)); }
			void SetProjectionMatrix(const Matrix4x4F& _ProjectionMatrix)
			{ MatrixUBOData->Write(_ProjectionMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, Projection)); }

			void SetInverseProjectionMatrix(const Matrix4x4F& _InverseProjectionMatrix)
			{ MatrixUBOData->Write(_InverseProjectionMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, InverseProjection)); }
			void SetInverseViewingMatrix(const Matrix4x4F& _InverseViewingMatrix)
			{ MatrixUBOData->Write(_InverseViewingMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, InverseViewing)); }
			void SetCofactorModelMatrix(const Matrix4x4F& _CofactorModelMatrix)
			{ MatrixUBOData->Write(_CofactorModelMatrix.data(), sizeof(Matrix4x4F), offsetof(FMatrixUBOLayout, CofactorModel)); }

			void SetLightSpaceTransform(const Matrix4x4F& _LightSpaceTransform)
			{ LightUBOData->Write(_LightSpaceTransform.data(), sizeof(Matrix4x4F), offsetof(FLightUBOLayout, LightSpaceTransform)); }

			Bool IsReady() const { return !InFlightFence.IsBlocking(); }

			static inline SharedPtr<FDescriptorSetLayout> MatrixUBODSLayout;
			static inline SharedPtr<FDescriptorSetLayout> LightUBODSLayout;
			static inline SharedPtr<FDescriptorSetLayout> SVColorTextureDSLayout;
			static inline SharedPtr<FSampler>             SVColorTextureSampler;
			static inline SharedPtr<FSampler>             SVShadowMapSampler;
		private:

			SharedPtr<FRenderTarget> BackgroundRTs;
			SharedPtr<FRenderTarget> ShadowRTs;
			SharedPtr<FRenderTarget> ForwardRTs;
			SharedPtr<FRenderTarget> PostprocessingRTs;

			SharedPtr<FImageView>     SVColorView;
			SharedPtr<FDescriptorSet> SVColorTexture;
			SharedPtr<FImage>         SVShadowMapImage;
			SharedPtr<FImageView>     SVShadowMapView;
			SharedPtr<FDescriptorSet> SVShadowMapTexture;
			SharedPtr<FImageView>     PostprocessInputView;
			SharedPtr<FDescriptorSet> PostprocessInputTexture;
			SharedPtr<FImageView>     PostprocessOutputView;
			SharedPtr<FDescriptorSet> PostprocessOutputTexture;

			SharedPtr<FBuffer>        MatrixUBOData;
			SharedPtr<FDescriptorSet> MatrixUBO;
			SharedPtr<FBuffer>        LightUBOData;
			SharedPtr<FDescriptorSet> LightUBO;

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
			        FSwizzle     _Swizzle      = {},
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
		static inline SharedPtr<const FSampler>
		GetGlobalTexture2DSampler() { return FFrameContext::SVColorTextureSampler; }

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

			FFrameContext::SVShadowMapSampler = RHI::CreateSampler(EFilter::Linear)
				->SetCompareMode(ECompareMode::Greater) // Reversed Z
				->SetAnisotropy(1.0)
				->SetBorderColor(EBorderColor::White_Float)
				->Build();

			FFrameContext::MatrixUBODSLayout = RHI::CreateDescriptorSetLayout()
				->AddBinding(0, EDescriptorType::UniformBuffer, 1, EShaderStage::Vertex)
				->Build();

			FFrameContext::LightUBODSLayout = RHI::CreateDescriptorSetLayout()
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

				Frame.MatrixUBOData = CreateMappedBuffer(sizeof(FMatrixUBOLayout), EBufferUsage::Uniform);
				Frame.MatrixUBO = RHI::CreateDescriptorSet(FFrameContext::MatrixUBODSLayout);
				Frame.MatrixUBO->WriteBuffer(0, Frame.MatrixUBOData, 0, Frame.MatrixUBOData->GetSize());

				Frame.LightUBOData = CreateMappedBuffer(sizeof(FLightUBOLayout), EBufferUsage::Uniform);
				Frame.LightUBO = RHI::CreateDescriptorSet(FFrameContext::LightUBODSLayout);
				Frame.LightUBO->WriteBuffer(0, Frame.LightUBOData, 0, Frame.LightUBOData->GetSize());

				auto ColorImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferSource);

				ImmeCmds->ConvertImageLayout(ColorImage, EVulkanImageLayout::ShaderReadOnly);

				Frame.SVShadowMapImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::S32_Float_Depth32,
					EImageAspect::Depth,
					EImageUsage::DepthStencilAttachment | EImageUsage::Sampled | EImageUsage::TransferSource);

				ImmeCmds->ConvertImageLayout(Frame.SVShadowMapImage, EVulkanImageLayout::DepthAttachment);

				auto PostprocessImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferSource);

				ImmeCmds->ConvertImageLayout(PostprocessImage, EVulkanImageLayout::ShaderReadOnly);

				auto DepthImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::S32_Float_Depth32,
					EImageAspect::Depth,
					EImageUsage::DepthStencilAttachment);

				ImmeCmds->ConvertImageLayout(DepthImage, EVulkanImageLayout::DepthAttachment);

				Frame.SVColorView    = ColorImage->CreateImageView();
				Frame.SVColorTexture = RHI::CreateDescriptorSet(FFrameContext::SVColorTextureDSLayout);
				Frame.SVColorTexture->WriteImage(0, Frame.SVColorView, FFrameContext::SVColorTextureSampler);

				Frame.SVShadowMapView    = Frame.SVShadowMapImage->CreateImageView(RHI::EImageLayout::ShaderReadOnly);
				Frame.SVShadowMapTexture = RHI::CreateDescriptorSet(FFrameContext::SVColorTextureDSLayout);
				Frame.SVShadowMapTexture->WriteImage(0, Frame.SVShadowMapView, FFrameContext::SVShadowMapSampler);

				Frame.PostprocessInputView     = Frame.SVColorView;
				Frame.PostprocessInputTexture  = Frame.SVColorTexture;
				Frame.PostprocessOutputView    = PostprocessImage->CreateImageView();
				Frame.PostprocessOutputTexture = RHI::CreateDescriptorSet(FFrameContext::SVColorTextureDSLayout);
				Frame.PostprocessOutputTexture->WriteImage(0, Frame.PostprocessOutputView, FFrameContext::SVColorTextureSampler);

				Frame.BackgroundRTs = FRenderTarget::Create()
					->AddColorImage(ColorImage)
					->AddDepthImage(DepthImage)
				    ->Confirm();

				Frame.ShadowRTs = FRenderTarget::Create()
					->AddDepthImage(Frame.SVShadowMapImage)
					->Confirm();

				Frame.ForwardRTs = FRenderTarget::Create()
					->AddColorImage(ColorImage)
					->AddDepthImage(DepthImage)
				    ->Confirm();

				Frame.PostprocessingRTs = FRenderTarget::Create()
					->AddColorImage(PostprocessImage)
				    ->Confirm();
			}

			for (auto& SwapchainImage : Vulkan->GetSwapchain().GetImages())
			{
				ImmeCmds->ConvertImageLayout(SwapchainImage,
					                         EImageLayout::Undefined,
					                         EImageLayout::Present,
					                         EImageAspect::Color,
					                         1,
					                         1);
			}
			ImmeCmds->StopRecording();

			ImmeCmds->SubmitAndWait();
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
			FFrameContext::SVShadowMapSampler->Destroy();
			FFrameContext::MatrixUBODSLayout->Destroy();
			FFrameContext::LightUBODSLayout->Destroy();
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
		VE_LOG_INFO("Creating a new render pass ({}).", typeid(T).name());
		auto NewRenderPass = CreateSharedPtr<T>();
		Array<SharedPtr<FRenderTarget>> RenderTargets;

		const char* RenderPassType{};
		switch (FRenderPass::EType(NewRenderPass->GetType()))
		{
		case FRenderPass::EType::Background:
		{
			RenderPassType = "Background";
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].BackgroundRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::Shadow:
		{
			RenderPassType = "Shadow";
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].ShadowRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::DefaultForward:
		{
			RenderPassType = "DefaultForward";
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].ForwardRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::Postprocessing:
		{
			RenderPassType = "Postprocessing";
			RenderTargets.resize(Frames.size());
			for (UInt8 Idx = 0; Idx < RenderTargets.size(); ++Idx)
			{ RenderTargets[Idx] = Frames[Idx].PostprocessingRTs; }

			NewRenderPass->Build(FFrameContext::RenderArea, RenderTargets);
			break;
		}
		case FRenderPass::EType::Overlay:
		{
			RenderPassType = "Overlay";
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
			RenderPassType = "Customized";
			VE_LOG_FATAL("Customized Render Pass WIP...");
		default:
			RenderPassType = "Unknown";
			VE_LOG_FATAL("Failed to create the renderpass -- Unknown RenderPass Type!");
		}
		
		const auto& RenderArea = NewRenderPass->GetRenderArea();
		VE_LOG_INFO("Successfully created {} (handle:{}, type:{}, render area:[{},{},{},{}]).",
			typeid(T).name(),
			(Address)(NewRenderPass->GetHandle()),
			RenderPassType,
			RenderArea.offset.x, RenderArea.offset.y, RenderArea.extent.width, RenderArea.extent.height);

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
			VE_LOG_FATAL("WIP: Swapchain Recreation!");
		}

		return CurrentFrame;
	}

	void RHI::
	RenderAndPresentCurrentFrame()
	{
		auto& CurrentFrame = GetCurrentFrame();

		auto& ResourceCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Resource];
		ResourceCommandBuffer->StopRecording();
		ResourceCommandBuffer->Submit(nullptr, 0, nullptr,
			                         (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Resource]), 1);

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
		GraphicsCommandBuffer->Submit(GraphicsWaitSemaphores.data(), UInt32(GraphicsWaitSemaphores.size()),
			                          GraphicsWaitStages.data(),
			                         (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Graphics]), 1);

		auto& EditorCommandBuffer = CurrentFrame.CommandBuffers[FFrameContext::Editor];
		VE_ASSERT(FCommandBuffer::EStatus::ReadyToSubmit == EditorCommandBuffer->GetStatus()) //Stop Recording at Editor
		static Segment<EVulkanGraphicsPipelineStage, 1> EditorWaitStages
		{
			EVulkanGraphicsPipelineStage::PipelineTop
		};
		EditorCommandBuffer->Submit((VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Graphics]), 1,
			                        EditorWaitStages.data(),
			                        (VkSemaphore*)(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Editor]), 1,
		                            &CurrentFrame.InFlightFence);
		try
		{
			Vulkan->GetSwapchain().Present(&CurrentFrame.CommandsFinishedSemaphores[FFrameContext::Editor], 1);
		}
		catch (const SSwapchainRecreation& Signal)
		{
			VE_LOG_DEBUG("Recreating swapchain...");
			VE_LOG_FATAL("WIP: Swapchain Recreation!");
		}
	}

	SharedPtr<RHI::FDescriptorSetLayout> RHI::
	CreateDescriptorSetLayout()
	{ 
		auto NewDescriptorSetLayout = FDescriptorSetLayout::Create();
		VE_LOG_DEBUG("Created a new descriptor set layout (need to be built).");
		return NewDescriptorSetLayout;
	}

	SharedPtr<RHI::FDescriptorSet> RHI::
	CreateDescriptorSet(SharedPtr<const FDescriptorSetLayout> _SetLayout)
	{
		auto NewDescriptorSet = GlobalDescriptorPool->CreateDescriptorSet(_SetLayout);
		VE_LOG_DEBUG("Created a new descriptor set (handle:{}, layout:{}).",
			(Address)(NewDescriptorSet->GetHandle()), (Address)(_SetLayout->GetHandle()));
		return NewDescriptorSet;
	}

	SharedPtr<RHI::FSampler> RHI::
	CreateSampler(EFilter _Filter, ESamplerAddressMode _AddressMode/* = ESamplerAddressMode::ClampToEdge*/)
	{
		auto NewSampler = FSampler::Create(_Filter, _AddressMode);
		VE_LOG_DEBUG("Created a new sampler (need to be built).");
		return NewSampler;
	}

	SharedPtr<RHI::FGraphicsCommandBuffer> RHI::
	CreateGraphicsCommandBuffer(ECommandLevel _Level/* = ECommandLevel::Primary*/)
	{
		auto NewResettableGraphicsCMD = ResetableGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level);
		VE_LOG_DEBUG("Created a new resettable graphics command buffer (handle:{}, level:{})",
			(Address)(NewResettableGraphicsCMD->GetHandle()), UInt32(_Level));
		return NewResettableGraphicsCMD;
	}

	SharedPtr<RHI::FGraphicsCommandBuffer> RHI::
	CreateOneTimeGraphicsCommandBuffer(ECommandLevel _Level/* = ECommandLevel::Primary*/)
	{ 
		auto NewTransientGraphicsCMD = TransientGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level);
		VE_LOG_DEBUG("Created a new transient graphics command buffer (handle:{}, level:{})",
			(Address)(NewTransientGraphicsCMD->GetHandle()), UInt32(_Level));
		return NewTransientGraphicsCMD;
	}

	SharedPtr<RHI::FImage> RHI::
	CreateImage(EImageType   _Type,
		        FExtent3D    _Extent,
		        EFormat      _Format,
		        EImageAspect _Aspects,
		        EImageUsage  _Usages,
		        FSwizzle     _Swizzle     /* = {} */,
		        EImageTiling _Tiling      /* = EImageTiling::Optimal*/,
		        ESampleRate  _SampleRate  /* = ESampleRate::X1*/,
		        UInt8        _MipmapLevels/* = 1*/,
		        UInt8        _ArrayLayers /* = 1*/,
		        ESharingMode _SharingMode /* = ESharingMode::Exclusive*/,
		        EMemoryUsage _Location    /* = EMemoryUsage::Auto*/)
	{
		auto NewImage = Vulkan->GetAllocator().CreateImage(
			_Type, _Extent, _Format, _Aspects, _Usages,
			_Swizzle, _Tiling, _SampleRate,
			_MipmapLevels, _ArrayLayers,
			_SharingMode, _Location);

		VE_LOG_DEBUG("Created a new image (handle:{}, format:{}, size:{}, extent:[{}, {}, {}], swizzle:[{},{},{},{}]).",
			(Address)(NewImage->GetHandle()), UInt32(_Format), NewImage->GetSize(),
			_Extent.width, _Extent.height, _Extent.depth,
			UInt32(_Swizzle.R), UInt32(_Swizzle.G), UInt32(_Swizzle.B), UInt32(_Swizzle.A));

		return NewImage;
	}

	SharedPtr<RHI::FBuffer> RHI::
	CreateBuffer(UInt64       _Size,
		         EBufferUsage _Usages,
		         ESharingMode _SharingMode     /*= EVulkanSharingMode::Exclusive*/,
		         EMemoryUsage _Location        /*= EMemoryUsage::Auto*/,
		         UInt32       _AllocationFlags /*= 0x0*/)
	{
		auto NewBuffer = Vulkan->GetAllocator().CreateBuffer(_Size, _Usages, _SharingMode, _Location, _AllocationFlags);
		VE_LOG_DEBUG("Created a new buffer (handle:{}, size:{})", (Address)(NewBuffer->GetHandle()), _Size);
		return NewBuffer;
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
		FFence NewFence{_Status};
		VE_LOG_DEBUG("Created a new fence (handle:{}, signaled:{}).",
			(Address)(NewFence.GetHandle()), (_Status == FFence::EStatus::Signaled));
		return NewFence;
	}

	RHI::FSemaphore RHI::
	CreateSemaphore()
	{
		FSemaphore NewSemaphore{};
		VE_LOG_DEBUG("Created a new semaphore (handle:{}).", (Address)(NewSemaphore.GetHandle()));
		return NewSemaphore;
	}

	SharedPtr<RHI::FSPIRVShader> RHI::
	CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize)
	{
		auto NewShader = FSPIRVShader::Create(_ShaderStage, _SPIRVCode, _CodeSize);
		VE_LOG_DEBUG("Created a new shader (handle:{}, stage:{}, size: {}).",
			(Address)(NewShader->GetHandle()), UInt32(_ShaderStage), _CodeSize);
		return NewShader;
	}

	SharedPtr<RHI::FPipelineLayout> RHI::
	CreatePipelineLayout()
	{
		auto NewPipelineLayout = FPipelineLayout::Create();
		VE_LOG_DEBUG("Created a new pipline layout (need to be built).");
		return NewPipelineLayout;
	}

	SharedPtr<RHI::FRenderPipelineSetting> RHI::
	CreateRenderPipelineSetting()
	{
		auto NewRenderPipelineSetting = FRenderPipelineSetting::Create();
		VE_LOG_DEBUG("Created a new render pipline setting.");
		return NewRenderPipelineSetting;
	}

} // namespace VE