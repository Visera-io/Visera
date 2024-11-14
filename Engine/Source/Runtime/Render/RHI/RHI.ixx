module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI;
import Visera.Engine.Runtime.Render.RHI.Vulkan;

export namespace VE { namespace Runtime
{	
	#define ENUM_BIT(Flag, Bit) static constexpr VkFlags Flag = Bit;

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
		using ShaderStages		= VulkanShaderStages::Option;
		using RenderPass		= VulkanRenderPass;
		using AccessPermissions	= VulkanAccessPermissions;
		using PipelineStages	= VulkanPipelineStages;
		using ImageLayouts		= VulkanImageLayouts;
		using PipelineStages	= VulkanPipelineStages;
		using AttachmentIO		= VulkanAttachmentIO;

	public:
		static inline auto
		GetResetableGraphicsCommandPool()	-> CommandPool& { return ResetableGraphicsCommandPool; }
		static inline auto
		GetTransientGraphicsCommandPool()	-> CommandPool& { return TransientGraphicsCommandPool; }
		static inline auto
		GetSwapchain()						-> const VulkanSwapchain& { return Vulkan::Swapchain; }
		static inline auto
		CreateFence()						-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(); }
		static inline auto
		CreateSignaledFence()				-> SharedPtr<Fence> { return CreateSharedPtr<Fence>(true); }
		static inline auto
		CreateSemaphore()					-> SharedPtr<Semaphore> { return CreateSharedPtr<Semaphore>(); }
		static inline auto
		CreateSignaledSemaphore()			-> SharedPtr<Semaphore> { Assert(False, "Not Supported by Vulkan"); return CreateSharedPtr<Semaphore>(true); }
		static inline auto
		CreateShader(ShaderStages Stage, const Array<Byte>& ShadingCode) { return CreateSharedPtr<VulkanShader>(Stage, ShadingCode);}

	private:
		static inline
		const VulkanSwapchain::Frame&
		WaitForSwapchain() throw(VulkanSwapchain::RecreateSignal) { return Vulkan::Swapchain.WaitForNextFrame(); }

		static inline void
		PresentSwapchain(const Semaphore& ExternalCommandBuffer) throw(VulkanSwapchain::RecreateSignal) {  Vulkan::Swapchain.Present(ExternalCommandBuffer); }

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
		}
		static void
		Terminate()
		{
			TransientGraphicsCommandPool.Destroy();
			ResetableGraphicsCommandPool.Destroy();
			Vulkan::Terminate();
		}
	};

} } // namespace VE::Runtime