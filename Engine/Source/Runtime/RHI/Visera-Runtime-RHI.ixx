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
		using FDescriptorBinding	= FVulkanDescriptorSetLayout::FBinding;
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
		using FRenderTarget			= FVulkanRenderTarget;
		using FRenderPipeline		= FVulkanRenderPipeline;	
		using FPipelineLayout		= FVulkanPipelineLayout;
		using FRenderPipelineSetting= FVulkanRenderPipelineSetting;
		using FPushConstantRange	= FVulkanPipelineLayout::FPushConstantRange;

		using ESharingMode			= EVulkanSharingMode;
		using ESampleRate			= EVulkanSampleRate;
		using EQueueFamily			= EVulkanQueueFamily;
		using ECommandPoolType		= EVulkanCommandPoolType;
		using ECommandLevel			= EVulkanCommandLevel;
		using EShaderStage			= EVulkanShaderStage;
		using EAccessibility		= EVulkanAccessibility;
		using EPipelineStage		= EVulkanPipelineStage;
		using EMemoryUsage			= EVulkanMemoryUsage;
		using EBufferUsage			= EVulkanBufferUsage;
		using EPipelineStage		= EVulkanPipelineStage;
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
	public: 
		VE_API CreateRenderTargets(const Array<SharedPtr<FImage>>& _ColorImages, SharedPtr<FImage> _DepthImage = nullptr) -> SharedPtr<FRenderTarget> { return CreateSharedPtr<FRenderTarget>(_ColorImages, _DepthImage); }
		VE_API CreateDescriptorSetLayout(const Array<FDescriptorBinding> _Bindings)			-> SharedPtr<FDescriptorSetLayout> { return CreateSharedPtr<FDescriptorSetLayout>(_Bindings); }
		VE_API CreateDescriptorSet(SharedPtr<FDescriptorSetLayout> _SetLayout)				-> SharedPtr<FDescriptorSet> { return GlobalDescriptorPool.CreateDescriptorSet(_SetLayout);		}
		VE_API CreateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)			-> SharedPtr<FCommandBuffer> { return ResetableGraphicsCommandPool->CreateCommandBuffer(_Level); }
		VE_API CreateImmediateCommandBuffer(ECommandLevel _Level = ECommandLevel::Primary)	-> SharedPtr<FCommandBuffer> { return TransientGraphicsCommandPool->CreateCommandBuffer(_Level); }
		VE_API CreateImage(EImageType _Type, FExtent3D _Extent, EFormat _Format, EImageAspect _Aspects, EImageUsage _Usages, EImageTiling _Tiling = EImageTiling::Optimal,ESampleRate _SampleRate = ESampleRate::X1, UInt8 _MipmapLevels = 1,UInt8 _ArrayLayers = 1, ESharingMode	_SharingMode = ESharingMode::Exclusive,EMemoryUsage	_Location = EMemoryUsage::Auto)->SharedPtr<FImage> { return Vulkan->Allocator.CreateImage(_Type, _Extent, _Format, _Aspects, _Usages, _Tiling, _SampleRate, _MipmapLevels, _ArrayLayers, _SharingMode, _Location); }
		VE_API CreateBuffer(UInt64 _Size, EBufferUsage _Usages, ESharingMode _SharingMode = EVulkanSharingMode::Exclusive, EMemoryUsage _Location = EMemoryUsage::Auto) -> SharedPtr<FBuffer> { return Vulkan->Allocator.CreateBuffer(_Size, _Usages, _SharingMode, _Location); }
		VE_API CreateFence()																-> SharedPtr<FFence>		 { return CreateSharedPtr<FFence>();						}
		VE_API CreateSignaledFence()														-> SharedPtr<FFence>		 { return CreateSharedPtr<FFence>(true);					}
		VE_API CreateSemaphore()															-> SharedPtr<FSemaphore>	 { return CreateSharedPtr<FSemaphore>();					}
		VE_API CreateShader(EShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize) -> SharedPtr<FShader> { return CreateSharedPtr<FShader>(_ShaderStage, _SPIRVCode, _CodeSize); }

		VE_API WaitIdle()	-> void				{ Vulkan->Device.WaitIdle(); }

		VE_API GetSwapchainFrameCount()	-> UInt32		  { return Vulkan->Swapchain.GetFrameCount(); }
		VE_API GetSwapchainCursor()		-> UInt32		  { return Vulkan->Swapchain.GetCursor(); }
		VE_API GetAPI()					-> const FVulkan* { return Vulkan; }

	private:
		static inline FVulkan*			 Vulkan;
		//[TODO]: ThreadSafe Paradigm
		static inline FDescriptorPool			GlobalDescriptorPool{};
		static inline SharedPtr<FCommandPool>	ResetableGraphicsCommandPool{};
		static inline SharedPtr<FCommandPool>	TransientGraphicsCommandPool{};
		
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
		//WaitForCurrentFrame() throw(SSwapchainRecreation)
		//{
		//	auto& CurrentFrame = GetCurrentFrame();
		//	CurrentFrame.Fence_Rendering.Wait();
		//	Vulkan->Swapchain.WaitForCurrentImage(CurrentFrame.Semaphore_ReadyToRender, nullptr);
		//	CurrentFrame.Fence_Rendering.Lock(); //Reset to Unsignaled (Lock)
		//}

		//static inline void
		//PresentCurrentFrame()	throw(SSwapchainRecreation)
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
			ResetableGraphicsCommandPool = CreateSharedPtr<FCommandPool>
				(ECommandPoolType::Resetable, EQueueFamily::Graphics);
			TransientGraphicsCommandPool = CreateSharedPtr<FCommandPool>
				(ECommandPoolType::Transient, EQueueFamily::Graphics);
		}
		static void
		Tick()
		{

		}
		static void
		Terminate()
		{
			WaitIdle();
			TransientGraphicsCommandPool.reset();
			ResetableGraphicsCommandPool.reset();
			GlobalDescriptorPool.Destroy();
			delete Vulkan;
		}
	};

} } // namespace VE::Runtime