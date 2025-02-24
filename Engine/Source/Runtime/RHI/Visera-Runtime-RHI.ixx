module;
#include <Visera.h>
#include "Vulkan/VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI;
import Visera.Runtime.RHI.Vulkan;

import Visera.Core.Signal;
import Visera.Core.Time;

export namespace VE
{
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
		using EFilter				= EVulkanFilter;
		using EPresentMode			= EVulkanPresentMode;

		using SSwapchainRecreation = FVulkanSwapchain::SRecreation;
	
		enum class ESystemRenderTarget{ Color, Depth };
		class FFrameContext
		{
			friend class RHI;
		public:
			auto GetGraphicsCommandBuffer() -> SharedPtr<FGraphicsCommandBuffer> { return GraphicsCommandBuffer; }
			auto GetEditorCommandBuffer()   -> SharedPtr<FGraphicsCommandBuffer> { return EditorCommandBuffer;   }

			Bool IsReady() const { return !InFlightFence.IsBlocking(); }

		private:
			static inline FRenderArea RenderArea{ {0,0},{ 1280,800 } }; //[FIXME]: Read from Config
			SharedPtr<FRenderTarget> RenderTarget = CreateSharedPtr<FRenderTarget>();

			SharedPtr<FGraphicsCommandBuffer> GraphicsCommandBuffer	= RHI::CreateGraphicsCommandBuffer();
			SharedPtr<FGraphicsCommandBuffer> EditorCommandBuffer	= RHI::CreateGraphicsCommandBuffer();
			FSemaphore GraphicsCommandsFinishedSemaphore= RHI::CreateSemaphore();
			FSemaphore EditorCommandsFinishedSemaphore	= RHI::CreateSemaphore();

			FSemaphore SwapchainReadySemaphore			= RHI::CreateSemaphore();
			FFence	   InFlightFence					= RHI::CreateFence(FFence::EStatus::Signaled);
		};

	public:
		VE_API CreateDescriptorSetLayout(const Array<FDescriptorBinding> _Bindings)			-> SharedPtr<FDescriptorSetLayout> { return CreateSharedPtr<FDescriptorSetLayout>(_Bindings); }
		VE_API CreateDescriptorSet(SharedPtr<FDescriptorSetLayout> _SetLayout)				-> SharedPtr<FDescriptorSet> { return GlobalDescriptorPool.CreateDescriptorSet(_SetLayout);		}
		VE_API CreateGraphicsCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return ResetableGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		VE_API CreateImmediateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FGraphicsCommandBuffer> { return TransientGraphicsCommandPool->CreateGraphicsCommandBuffer(_Level); }
		VE_API CreateImage(EImageType _Type, FExtent3D _Extent, EFormat _Format, EImageAspect _Aspects, EImageUsage _Usages, EImageTiling _Tiling = EImageTiling::Optimal,ESampleRate _SampleRate = ESampleRate::X1, UInt8 _MipmapLevels = 1,UInt8 _ArrayLayers = 1, ESharingMode	_SharingMode = ESharingMode::Exclusive,EMemoryUsage	_Location = EMemoryUsage::Auto)->SharedPtr<FImage> { return Vulkan->Allocator.CreateImage(_Type, _Extent, _Format, _Aspects, _Usages, _Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location); }
		VE_API CreateBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer> { return Vulkan->Allocator.CreateBuffer(_Size, _Usages, _SharingMode, _Location); }
		VE_API CreateFence(FFence::EStatus _Status = FFence::EStatus::Blocking)				-> FFence		 { return FFence{_Status};	}
		VE_API CreateSemaphore()															-> FSemaphore	 { return FSemaphore();	}
		VE_API CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize) -> SharedPtr<FShader> { return CreateSharedPtr<FShader>(_ShaderStage, _SPIRVCode, _CodeSize); }
		template<typename T> static inline
		auto   CreateRenderPass() -> SharedPtr<T>;

		VE_API WaitFrameReady()					-> FFrameContext&;
		VE_API GetCurrentFrame()				-> FFrameContext&;
		VE_API RenderAndPresentCurrentFrame()	-> void;
		VE_API GetFPS()							-> UInt32 { return FPS; }

		VE_API GetSwapchainFrameCount()	-> UInt32		  { return Vulkan->Swapchain.GetFrameCount(); }
		VE_API GetSwapchainCursor()		-> UInt32		  { return Vulkan->Swapchain.GetCursor(); }
		VE_API GetSwapchainFormat()		-> EFormat		  { return Vulkan->Swapchain.GetFormat(); }

		VE_API WaitDeviceIdle()			-> void		{ Vulkan->Device.WaitIdle(); }
		VE_API GetAPI()					-> const FVulkan* { return Vulkan; }
		VE_API GetGlobalDescriptorPool()-> const FDescriptorPool& { return GlobalDescriptorPool; }

	private:
		static inline FVulkan*							Vulkan;
		//[TODO]: ThreadSafe Paradigm
		static inline FDescriptorPool					GlobalDescriptorPool{};
		static inline SharedPtr<FGraphicsCommandPool>	ResetableGraphicsCommandPool{};
		static inline SharedPtr<FGraphicsCommandPool>	TransientGraphicsCommandPool{};

		static inline Array<FFrameContext> Frames;
		static inline UInt32 FPS{ 0 };

	private:
		static void inline
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

			auto ImmeCmds = CreateImmediateCommandBuffer();
			ImmeCmds->StartRecording();

			for (auto& Frame : Frames)
			{
				auto ColorImage = CreateImage(
					EImageType::Image2D,
					{ FFrameContext::RenderArea.extent.width, FFrameContext::RenderArea.extent.height, 1 },
					EFormat::U32_Normalized_R8_G8_B8_A8,
					EImageAspect::Color,
					EImageUsage::ColorAttachment | EImageUsage::TransferSource,
					EImageTiling::Optimal,
					ESampleRate::X1
				);
				ImmeCmds->ConvertImageLayout(ColorImage, EVulkanImageLayout::ColorAttachment);
				Frame.RenderTarget->AddColorImage(ColorImage);
				
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
			for (auto& SwapchainImage : Vulkan->Swapchain.GetImages())
			{
				VkImageMemoryBarrier SwapchainTransferBarrier
				{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.srcAccessMask = AutoCast(EAccessibility::None),
					.dstAccessMask = AutoCast(EAccessibility::None),
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
					ImmeCmds->Handle,
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
					.WaitStages				= EGraphicsPipelineStage::PipelineTop,
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
			GlobalDescriptorPool.Destroy();
			delete Vulkan;
		}
	};

	RHI::FFrameContext& RHI::
	GetCurrentFrame()
	{
		return Frames[GetSwapchainCursor()];
	}

	template<typename T>
	SharedPtr<T> RHI::
	CreateRenderPass()
	{
		auto NewRenderPass = CreateSharedPtr<T>();

		Array<SharedPtr<FRenderTarget>> RenderTargets;
		RenderTargets.resize(Frames.size());
		for (UInt8 Idx = 0; Idx < RenderTargets.size();++Idx)
		{
			RenderTargets[Idx] = Frames[Idx].RenderTarget;
		}
		NewRenderPass->Create(FFrameContext::RenderArea, RenderTargets);

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
			Vulkan->Swapchain.WaitForNextImage(CurrentFrame.SwapchainReadySemaphore);

			CurrentFrame.InFlightFence.Block();
			CurrentFrame.GraphicsCommandBuffer->Status = FGraphicsCommandBuffer::EStatus::Idle;
			CurrentFrame.GraphicsCommandBuffer->Reset();
			CurrentFrame.GraphicsCommandBuffer->StartRecording();

			CurrentFrame.EditorCommandBuffer->Status = FGraphicsCommandBuffer::EStatus::Idle;
			CurrentFrame.EditorCommandBuffer->Reset();
			CurrentFrame.EditorCommandBuffer->StartRecording();
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
		try
		{
			CurrentFrame.EditorCommandBuffer->StopRecording();
			CurrentFrame.EditorCommandBuffer->Submit(
				FCommandSubmitInfo
				{
					.WaitSemaphoreCount		= 1,
					.pWaitSemaphores		= &CurrentFrame.SwapchainReadySemaphore,
					.WaitStages				= EGraphicsPipelineStage::FragmentShader,
					.SignalSemaphoreCount	= 1,
					.pSignalSemaphores		= &CurrentFrame.EditorCommandsFinishedSemaphore,
					.SignalFence			= nullptr,
				});

			//[FIXME]: Testing
			VkImageMemoryBarrier SwapchainTransferBarrier
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.srcAccessMask = AutoCast(EAccessibility::R_Memory),
				.dstAccessMask = AutoCast(EAccessibility::W_Transfer),
				.oldLayout = AutoCast(EImageLayout::Present),
				.newLayout = AutoCast(EImageLayout::TransferDestination),
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = Vulkan->Swapchain.GetCurrentImage(),
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
				CurrentFrame.GraphicsCommandBuffer->Handle,
				AutoCast(EGraphicsPipelineStage::PipelineTop),
				AutoCast(EGraphicsPipelineStage::Transfer),
				0x0,		// Dependency Flags
				0, nullptr,	// Memory Barrier
				0, nullptr,	// Buffer Memory Barrier
				1,
				&SwapchainTransferBarrier);

			VkImageBlit BlitInfo
			{
				.srcSubresource
				{
					.aspectMask		= AutoCast(CurrentFrame.RenderTarget->ColorImages[0]->GetAspects()),
					.mipLevel		= 0,
					.baseArrayLayer = 0,
					.layerCount = CurrentFrame.RenderTarget->ColorImages[0]->GetArrayLayers(),
				},
				.srcOffsets
				{
					{0, 0, 0},
					{Int32(CurrentFrame.RenderTarget->ColorImages[0]->GetExtent().width),
					 Int32(CurrentFrame.RenderTarget->ColorImages[0]->GetExtent().height),
					 Int32(CurrentFrame.RenderTarget->ColorImages[0]->GetExtent().depth)},
				},
				.dstSubresource
				{
					.aspectMask = AutoCast(EImageAspect::Color),
					.mipLevel		= 0,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				},
				.dstOffsets
				{
					{0, 0, 0},
					{Int32(Vulkan->Swapchain.GetExtent().width),
					 Int32(Vulkan->Swapchain.GetExtent().height),
					 1},
				}
			};
						
			//[FIXME]: RenderPass FinalLayout Invalid???
			CurrentFrame.GraphicsCommandBuffer->ConvertImageLayout(CurrentFrame.RenderTarget->ColorImages[0], EImageLayout::TransferSource);

			vkCmdBlitImage(
				CurrentFrame.GraphicsCommandBuffer->Handle,
				CurrentFrame.RenderTarget->ColorImages[0]->GetHandle(),
				AutoCast(CurrentFrame.RenderTarget->ColorImages[0]->GetLayout()),
				Vulkan->Swapchain.GetCurrentImage(),
				AutoCast(EImageLayout::TransferDestination),
				1,
				&BlitInfo,
				AutoCast(EFilter::Linear));

			//[FIXME]: RenderPass FinalLayout Invalid???
			CurrentFrame.GraphicsCommandBuffer->ConvertImageLayout(CurrentFrame.RenderTarget->ColorImages[0], EImageLayout::ColorAttachment);
			
			// [FIXME]: Currently clear in the EditorPass
			// CurrentFrame.GraphicsCommandBuffer->ClearColorImage(CurrentFrame.RenderTarget->ColorImages[0]);

			VkImageMemoryBarrier SwapchainPresentBarrier
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.srcAccessMask = AutoCast(EAccessibility::None),
				.dstAccessMask = AutoCast(EAccessibility::R_Memory),
				.oldLayout = AutoCast(EImageLayout::TransferDestination),
				.newLayout = AutoCast(EImageLayout::Present),
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = Vulkan->Swapchain.GetCurrentImage(),
				.subresourceRange
				{
					.aspectMask		= AutoCast(EImageAspect::Color),
					.baseMipLevel	= 0,
					.levelCount		= 1,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				}
			};
			vkCmdPipelineBarrier(
				CurrentFrame.GraphicsCommandBuffer->Handle,
				AutoCast(EGraphicsPipelineStage::Transfer),
				AutoCast(EGraphicsPipelineStage::PipelineBottom),
				0x0,		// Dependency Flags
				0, nullptr,	// Memory Barrier
				0, nullptr,	// Buffer Memory Barrier
				1,
				&SwapchainPresentBarrier);

			CurrentFrame.GraphicsCommandBuffer->StopRecording();
			CurrentFrame.GraphicsCommandBuffer->Submit(
				FCommandSubmitInfo
				{
					.WaitSemaphoreCount		= 1,
					.pWaitSemaphores		= &CurrentFrame.EditorCommandsFinishedSemaphore,//[FIXME]: Temp!
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

} // namespace VE