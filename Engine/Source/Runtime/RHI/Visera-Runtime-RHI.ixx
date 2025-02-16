module;
#include <Visera.h>
#include "Vulkan/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI;
import Visera.Runtime.RHI.Vulkan;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class ViseraRuntime;

	/* This is not a thread-safe RHI! */
	class RHI
	{
		VE_MODULE_MANAGER_CLASS(RHI);
		friend class ViseraRuntime;
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
		using FCommandSubmitInfo	= FVulkanCommandSubmitInfo;
		using FRenderArea			= FVulkanRenderArea;

		using ESharingMode			= EVulkanSharingMode;
		using ESampleRate			= EVulkanSampleRate;
		using EQueueFamily			= EVulkanQueueFamily;
		using ECommandPoolType		= EVulkanCommandPoolType;
		using ECommandLevel			= EVulkanCommandLevel;
		using EShaderStage			= EVulkanShaderStage;
		using EAccessibility		= EVulkanAccess;
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
		using EPresentMode			= EVulkanPresentMode;

		using SSwapchainRecreation = FVulkanSwapchain::SRecreation;
	
		enum class ESystemRenderTarget{ Color, Depth };
		class FFrameContext
		{
			friend class RHI;
		public:
			auto GetGraphicsCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer> { return GraphicsCommandBuffer; }

		private:
			static inline FRenderArea RenderArea{ {0,0},{ 1920,1080 } };
			SharedPtr<FRenderTarget> RenderTarget = CreateSharedPtr<FRenderTarget>();

			SharedPtr<FGraphicsCommandBuffer> GraphicsCommandBuffer	= RHI::CreateGraphicsCommandBuffer();
			FSemaphore GraphicsCommandsFinishedSemaphore= RHI::CreateSemaphore();

			FSemaphore SwapchainReadySemaphore			= RHI::CreateSemaphore();
			FFence	   InFlightFence					= RHI::CreateFence(FFence::EStatus::Signaled);
		};

	public:
		VE_API CreateRenderPass(SharedPtr<FRenderPass> _RenderPass)							-> void;
		VE_API CreateDescriptorSetLayout(const Array<FDescriptorBinding> _Bindings)			-> SharedPtr<FDescriptorSetLayout> { return CreateSharedPtr<FDescriptorSetLayout>(_Bindings); }
		VE_API CreateDescriptorSet(SharedPtr<FDescriptorSetLayout> _SetLayout)				-> SharedPtr<FDescriptorSet> { return GlobalDescriptorPool.CreateDescriptorSet(_SetLayout);		}
		VE_API CreateGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return ResetableGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		VE_API CreateImmediateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return TransientGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		VE_API CreateImage(EImageType _Type, FExtent3D _Extent, EFormat _Format, EImageAspect _Aspects, EImageUsage _Usages, EImageTiling _Tiling = EImageTiling::Optimal,ESampleRate _SampleRate = ESampleRate::X1, UInt8 _MipmapLevels = 1,UInt8 _ArrayLayers = 1, ESharingMode	_SharingMode = ESharingMode::Exclusive,EMemoryUsage	_Location = EMemoryUsage::Auto)->SharedPtr<FImage> { return Vulkan->Allocator.CreateImage(_Type, _Extent, _Format, _Aspects, _Usages, _Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location); }
		VE_API CreateBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer> { return Vulkan->Allocator.CreateBuffer(_Size, _Usages, _SharingMode, _Location); }
		VE_API CreateFence(FFence::EStatus _Status = FFence::EStatus::Blocking)				-> FFence		 { return FFence{_Status};	}
		VE_API CreateSemaphore()															-> FSemaphore	 { return FSemaphore();	}
		VE_API CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize) -> SharedPtr<FShader> { return CreateSharedPtr<FShader>(_ShaderStage, _SPIRVCode, _CodeSize); }

		VE_API WaitNextFrame()					-> FFrameContext&;
		VE_API RenderAndPresentCurrentFrame()	-> void;

		VE_API GetSwapchainFrameCount()	-> UInt32		  { return Vulkan->Swapchain.GetFrameCount(); }
		VE_API GetSwapchainCursor()		-> UInt32		  { return Vulkan->Swapchain.GetCursor(); }

		VE_API WaitDeviceIdle()			-> void		{ Vulkan->Device.WaitIdle(); }
		VE_API GetAPI()					-> const FVulkan* { return Vulkan; }

	private:
		static inline FVulkan*							Vulkan;
		//[TODO]: ThreadSafe Paradigm
		static inline FDescriptorPool					GlobalDescriptorPool{};
		static inline SharedPtr<FGraphicsCommandPool>	ResetableGraphicsCommandPool{};
		static inline SharedPtr<FGraphicsCommandPool>	TransientGraphicsCommandPool{};

		static inline Array<FFrameContext> Frames;

	private:
		static void
		Bootstrap()
		{
			Vulkan = new FVulkan();
			GlobalDescriptorPool.Create(
				{
					{EDescriptorType::UniformBuffer,		1000},
					{EDescriptorType::StorageBuffer,		1000},
					{EDescriptorType::DynamicStorageBuffer, 1000},
					{EDescriptorType::DynamicUniformBuffer, 1000},
					{EDescriptorType::CombinedImageSampler, 1000},
					{EDescriptorType::InputAttachment,		1000},
					{EDescriptorType::SampledImage,			1000},
					{EDescriptorType::StorageImage,			1000},
					{EDescriptorType::StorageTexelBuffer,	1000},
					{EDescriptorType::UniformTexelBuffer,	1000},
					{EDescriptorType::Sampler,				1000},
				}, 10000);
			ResetableGraphicsCommandPool = CreateSharedPtr<FGraphicsCommandPool>(ECommandPoolType::Resetable);
			TransientGraphicsCommandPool = CreateSharedPtr<FGraphicsCommandPool>(ECommandPoolType::Transient);

			Frames.resize(GetSwapchainFrameCount());

			for (auto& Frame : Frames)
			{
				Frame.RenderTarget->AddColorImage(CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::TransferSource,
					EImageTiling::Optimal,
					ESampleRate::X1
				));
				Frame.RenderTarget->AddDepthImage(CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::S32_Float_Depth32,
					EImageAspect::Depth,
					EImageUsage::DepthStencilAttachment,
					EImageTiling::Optimal,
					ESampleRate::X1
				));
			}
		}

		static void
		Tick()
		{

		}

		static void
		Terminate()
		{
			WaitDeviceIdle();
			Frames.clear();
			TransientGraphicsCommandPool.reset();
			ResetableGraphicsCommandPool.reset();
			GlobalDescriptorPool.Destroy();
			delete Vulkan;
		}
	};

	void RHI::
	CreateRenderPass(SharedPtr<FRenderPass> _RenderPass)	
	{
		Array<SharedPtr<FRenderTarget>> RenderTargets;
		RenderTargets.resize(Frames.size());
		for (UInt8 Idx = 0; Idx < RenderTargets.size();++Idx)
		{
			RenderTargets[Idx] = Frames[Idx].RenderTarget;
		}
		_RenderPass->Create(FFrameContext::RenderArea, RenderTargets);
	}

	RHI::FFrameContext& RHI::
	WaitNextFrame()
	{
		auto& NextFrame = Frames[GetSwapchainCursor()];
		NextFrame.InFlightFence.Wait();
		NextFrame.GraphicsCommandBuffer->Status = FGraphicsCommandBuffer::EStatus::Idle;
		NextFrame.GraphicsCommandBuffer->Reset();
		
		try
		{
			Vulkan->Swapchain.WaitForNextImage(NextFrame.SwapchainReadySemaphore);

			NextFrame.InFlightFence.Block();
			NextFrame.GraphicsCommandBuffer->StartRecording();
		}
		catch (const SSwapchainRecreation& Signal)
		{
			throw SRuntimeError("WIP: Swapchain Recreation!");
		}

		return NextFrame;
	}

	void RHI::
	RenderAndPresentCurrentFrame()
	{
		auto& CurrentFrame = Frames[GetSwapchainCursor()];
		try
		{
			CurrentFrame.GraphicsCommandBuffer->StopRecording();
			CurrentFrame.GraphicsCommandBuffer->Submit(
				FCommandSubmitInfo
				{
					.WaitSemaphoreCount		= 1,
					.pWaitSemaphores		= &CurrentFrame.SwapchainReadySemaphore,
					.WaitStages				= EGraphicsPipelineStage::PipelineTop,
					.SignalSemaphoreCount	= 1,
					.pSignalSemaphores		= &CurrentFrame.GraphicsCommandsFinishedSemaphore,
					.SignalFence			= &CurrentFrame.InFlightFence,
				});

			Vulkan->Swapchain.Present(&CurrentFrame.GraphicsCommandsFinishedSemaphore, 1);
		}
		catch (const SSwapchainRecreation& Signal)
		{
			throw SRuntimeError("WIP: Swapchain Recreation!");
		}
	}

} } // namespace VE::Runtime