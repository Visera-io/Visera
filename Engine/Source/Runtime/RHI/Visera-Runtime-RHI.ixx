module;
#include <Visera.h>
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
		using FCommandPool			= FVulkanCommandPool;
		using FCommandBuffer		= FVulkanCommandBuffer;
		using FDescriptorPool		= FVulkanDescriptorPool;
		using FDescriptorSet		= FVulkanDescriptorSet;
		using FDescriptorSetLayout  = FVulkanDescriptorSetLayout;
		using FFence				= FVulkanFence;
		using FShader				= FVulkanShader;
		using FBuffer				= FVulkanBuffer;
		using FImage				= FVulkanImage;
		using FExtent2D				= FVulkanExtent2D;
		using FExtent3D				= FVulkanExtent3D;
		using FOffset2D				= FVulkanOffset2D;
		using FOffset3D				= FVulkanOffset3D;
		using FFramebuffer			= FVulkanFramebuffer;
		using FRenderPass			= FVulkanRenderPass;
		using FRenderPassLayout		= FVulkanRenderPassLayout;
		using FRenderPassResource	= FVulkanRenderPassResource;
		using FRenderPipeline		= FVulkanRenderPipeline;	
		using FPipelineLayout		= FVulkanPipelineLayout;
		using FRenderPipelineLayout	= FVulkanRenderPipelineLayout;

		using ESampleRate			= ESampleRate;
		using EQueueFamily			= EQueueFamily;
		using ECommandPool			= ECommandPool;
		using ECommandLevel			= ECommandLevel;
		using EShaderStage			= EShaderStage;
		using EAccessibility		= EAccessibility;
		using EPipelineStage		= EPipelineStage;
		using EMemoryUsage			= EMemoryUsage;
		using EBufferUsage			= EBufferUsage;
		using EPipelineStage		= EPipelineStage;
		using EAttachmentIO			= EAttachmentIO;
		using EDescriptorType		= EDescriptorType;
		using EImageType			= EImageType;
		using EImageLayout			= EImageLayout;
		using EImageUsage			= EImageUsage;
		using EImageViewType		= EImageViewType;
		using EImageAspect			= EImageAspect;
		using EImageTiling			= EImageTiling;
		using EFormat				= EFormat;
		using EPresentMode			= EPresentMode;

		using SwapchainRecreateSignal = FVulkanSwapchain::RecreateSignal;
	public:
		VE_API CreateRenderTargets(const Array<SharedPtr<FImage>>& _ColorImages, SharedPtr<FImage> _DepthImage = nullptr) -> SharedPtr<FRenderPassResource> { return CreateSharedPtr<FRenderPassResource>(_ColorImages, _DepthImage); }
		VE_API CreateDescriptorSet(SharedPtr<FVulkanDescriptorSetLayout> _SetLayout)		-> SharedPtr<FDescriptorSet> { return GlobalDescriptorPool.CreateDescriptorSet(_SetLayout);		}
		VE_API CreateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)			-> SharedPtr<FCommandBuffer> { return ResetableGraphicsCommandPool.CreateCommandBuffer(_Level); }
		VE_API CreateImmediateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FCommandBuffer> { return TransientGraphicsCommandPool.CreateCommandBuffer(_Level); }
		VE_API CreateImage(EImageType _Type, FExtent3D _Extent, EFormat _Format, EImageAspect _Aspects, EImageUsage _Usages, EImageTiling _Tiling = EImageTiling::Optimal,ESampleRate _SampleRate = ESampleRate::X1, UInt8 _MipmapLevels = 1,UInt8 _ArrayLayers = 1, ESharingMode	_SharingMode = ESharingMode::Exclusive,EMemoryUsage	_Location = EMemoryUsage::Auto)->SharedPtr<FImage> { return Vulkan->Allocator.CreateImage(_Type, _Extent, _Format, _Aspects, _Usages, _Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location); }
		VE_API CreateBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = ESharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer> { return Vulkan->Allocator.CreateBuffer(_Size, _Usages, _SharingMode, _Location); }
		VE_API CreateFence()																-> SharedPtr<FFence>		 { return CreateSharedPtr<FFence>();								}
		VE_API CreateSignaledFence()														-> SharedPtr<FFence>		 { return CreateSharedPtr<FFence>(true);							}
		VE_API CreateSemaphore()															-> SharedPtr<FSemaphore>	 { return CreateSharedPtr<FSemaphore>();							}
		VE_API CreateShader(EShaderStage Stage, const Array<Byte>& ShadingCode)				-> SharedPtr<FShader>		 { return CreateSharedPtr<FVulkanShader>(Stage, ShadingCode);		}

		VE_API SubmitCommandBuffer(SharedPtr<FCommandBuffer> _CommandBuffer);

		VE_API WaitIdle()	-> void				{ Vulkan->Device.WaitIdle(); }

		VE_API GetAPI()		-> const FVulkan*	{ return Vulkan; }

	private:
		static inline FVulkan*			 Vulkan;
		//[TODO]: ThreadSafe Paradigm
		static inline FDescriptorPool	GlobalDescriptorPool{};
		static inline FCommandPool		ResetableGraphicsCommandPool{};
		static inline FCommandPool		TransientGraphicsCommandPool{};
		
		//struct Frame
		//{
		//	FFence		Fence_Rendering{ True };
		//	FSemaphore	Semaphore_ReadyToRender;
		//	FSemaphore	Semaphore_ReadyToPresent;
		//	HashMap<String, SharedPtr<CommandContext>> CommandContexts;
		//};
		//static inline Array<Frame> Frames;

		//VE_API GetCurrentFrame() -> Frame& { return Frames[Vulkan->Swapchain.GetCursor()]; }

		//static inline void
		//WaitForCurrentFrame() throw(SwapchainRecreateSignal)
		//{
		//	auto& CurrentFrame = GetCurrentFrame();
		//	CurrentFrame.Fence_Rendering.Wait();
		//	Vulkan->Swapchain.WaitForCurrentImage(CurrentFrame.Semaphore_ReadyToRender, nullptr);
		//	CurrentFrame.Fence_Rendering.Lock(); //Reset to Unsignaled (Lock)
		//}

		//static inline void
		//PresentCurrentFrame()	throw(SwapchainRecreateSignal)
		//{
		//	auto& CurrentFrame = GetCurrentFrame();
		//	Vulkan->Swapchain.PresentCurrentImage(CurrentFrame.Semaphore_ReadyToPresent);
		//}

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
			ResetableGraphicsCommandPool.Create(EQueueFamily::Graphics, ECommandPool::Resetable);
			TransientGraphicsCommandPool.Create(EQueueFamily::Graphics, ECommandPool::Transient);
		}
		static void
		Tick()
		{

		}
		static void
		Terminate()
		{
			WaitIdle();
			TransientGraphicsCommandPool.Destroy();
			ResetableGraphicsCommandPool.Destroy();
			GlobalDescriptorPool.Destroy();
			delete Vulkan;
		}
	};

} } // namespace VE::Runtime