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
		using Shader			= VulkanShader;
		using ShaderStages		= VulkanShaderStages::Option;
		using RenderPass		= VulkanRenderPass;
		using AccessPermission	= VulkanAccessPermissions;
		using PipelineStages	= VulkanPipelineStages;
	public:
		static inline auto
		GetResetableGraphicsCommandPool()	-> VulkanCommandPool& { return ResetableGraphicsCommandPool; }
		static inline auto
		GetTransientGraphicsCommandPool()	-> VulkanCommandPool& { return TransientGraphicsCommandPool; }
		static inline auto
		CreateFence()						-> SharedPtr<VulkanFence> { return CreateSharedPtr<VulkanFence>(); }
		static inline auto
		CreateSignaledFence()				-> SharedPtr<VulkanFence> { return CreateSharedPtr<VulkanFence>(true); }
		static inline auto
		CreateSemaphore()					-> SharedPtr<VulkanSemaphore> { return CreateSharedPtr<VulkanSemaphore>(); }
		static inline auto
		CreateSignaledSemaphore()			-> SharedPtr<VulkanSemaphore> { return CreateSharedPtr<VulkanSemaphore>(true); }
		static inline auto
		CreateShader(ShaderStages Stage, const Array<Byte> ShadingCode) { return CreateSharedPtr<VulkanShader>(Stage, ShadingCode);
	}

	private:
		static inline VulkanCommandPool ResetableGraphicsCommandPool{};
		static inline VulkanCommandPool TransientGraphicsCommandPool{};

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