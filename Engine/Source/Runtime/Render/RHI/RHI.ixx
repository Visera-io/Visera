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
		using Shader = VulkanShader;
		using RenderPass = VulkanRenderPass;
		using ShaderStages	 = VkFlags;
		using PipelineStages = VkFlags;
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
		CreateShader(ShaderStages Stages, const Array<Byte> ShadingCode) { return CreateSharedPtr<VulkanShader>(VulkanShader::ShaderType(Stages), ShadingCode); }
	public:
		struct ShaderStage
		{
			ENUM_BIT(Vertex,	VulkanShader::ShaderType::Vertex)
			ENUM_BIT(Fragment,	VulkanShader::ShaderType::Fragment)
		};

		struct PipelineStage
		{
			ENUM_BIT(None,							VK_PIPELINE_STAGE_NONE);
			ENUM_BIT(All,							VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM);

			ENUM_BIT(PipelineTop,					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
			ENUM_BIT(DrawIndirect,					VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT)
			ENUM_BIT(VertexInput,					VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)
			ENUM_BIT(VertexShader,					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT)
			ENUM_BIT(TessellationControlShader,		VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT)
			ENUM_BIT(TessellationEvaluationShader,	VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT)
			ENUM_BIT(GeometryShader,				VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT)
			ENUM_BIT(FragmentShader,				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
			ENUM_BIT(EarlyFragmentTests,			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
			ENUM_BIT(LateFragmentTests,				VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
			ENUM_BIT(ColorAttachmentOutput,			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
			ENUM_BIT(ComputeShader,					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
			ENUM_BIT(Transfer,						VK_PIPELINE_STAGE_TRANSFER_BIT)
			ENUM_BIT(PipelineBottom,				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)

			ENUM_BIT(Host,							VK_PIPELINE_STAGE_HOST_BIT)
			ENUM_BIT(AllGraphics,					VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT)
			ENUM_BIT(AllCommands,					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
		};
	
		struct PipelineStageEXT
		{
			ENUM_BIT(None,							VK_PIPELINE_STAGE_NONE)		
			ENUM_BIT(All,							VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM)

			ENUM_BIT(TransformFeedback,				VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT)
			ENUM_BIT(ConditionalRendering,			VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT)
			ENUM_BIT(FragmentDensityProcess,		VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT)
			ENUM_BIT(TaskShader,					VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT)
			ENUM_BIT(MeshShader,					VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT)
			ENUM_BIT(CommandPreprocess,				VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT)
		};

		struct PipelineStageKHR
		{
			ENUM_BIT(None,							VK_PIPELINE_STAGE_NONE)		
			ENUM_BIT(All,							VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM)

			ENUM_BIT(AccelerationStructureBuild,	VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR)
			ENUM_BIT(RayTracingShader,				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR)
			ENUM_BIT(FragmentShadingRateAttachment, VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)
		};

		struct PipelineStageNV
		{
			ENUM_BIT(None,							VK_PIPELINE_STAGE_NONE)		
			ENUM_BIT(All,							VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM)

			ENUM_BIT(CommandPreprocess,				VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV)
			ENUM_BIT(ShadingRateImage,				VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV)
			ENUM_BIT(RayTracingShader,				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV)
			ENUM_BIT(AccelerationStructureBuild,	VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV)
			ENUM_BIT(TaskShader,					VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV)
			ENUM_BIT(MeshShader,					VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV)
		};

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