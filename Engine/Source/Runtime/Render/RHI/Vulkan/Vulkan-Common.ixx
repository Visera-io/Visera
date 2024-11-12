module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Common;

export namespace VE { namespace Runtime
{
    #define ENUM_BIT(Flag, Bit) static constexpr VkFlags Flag = Bit;

	struct VulkanAccessPermissions;
	struct VulkanShaderStages;
	struct VulkanPipelineStages;

    struct VulkanAccessPermissions
    {
        ENUM_BIT(None, VK_ACCESS_NONE)
        struct Read
        {
            ENUM_BIT(IndirectCommand,   VK_ACCESS_INDIRECT_COMMAND_READ_BIT)
            ENUM_BIT(Index,             VK_ACCESS_INDEX_READ_BIT)
            ENUM_BIT(VertexAttribute,   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
            ENUM_BIT(Uniform,           VK_ACCESS_UNIFORM_READ_BIT)
            ENUM_BIT(InputAttachment,   VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)
            ENUM_BIT(Shader,            VK_ACCESS_SHADER_READ_BIT)
            ENUM_BIT(ColorAttachment,   VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
            ENUM_BIT(DepthStencilAttachment, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT)
            ENUM_BIT(Transfer,          VK_ACCESS_TRANSFER_READ_BIT)
            ENUM_BIT(Host,              VK_ACCESS_HOST_READ_BIT)
            ENUM_BIT(Memory,            VK_ACCESS_MEMORY_READ_BIT)
        };
        struct Write
        {
            ENUM_BIT(Shader,            VK_ACCESS_SHADER_WRITE_BIT)
            ENUM_BIT(ColorAttachment,   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
            ENUM_BIT(DepthStencilAttachment, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
            ENUM_BIT(Transfer,          VK_ACCESS_TRANSFER_WRITE_BIT)
            ENUM_BIT(Host,              VK_ACCESS_HOST_WRITE_BIT)
            ENUM_BIT(Memory,            VK_ACCESS_MEMORY_WRITE_BIT)
        };

		VkAccessFlagBits Flags;
		VulkanAccessPermissions(VkFlags Flags) : Flags{ VkAccessFlagBits(Flags) } {}
    };

    struct VulkanShaderStages
	{
		ENUM_BIT(Vertex,	VK_SHADER_STAGE_VERTEX_BIT)
		ENUM_BIT(Fragment,	VK_SHADER_STAGE_FRAGMENT_BIT)

		VkShaderStageFlagBits Flags;
		VulkanShaderStages(VkFlags Flags) : Flags{ VkShaderStageFlagBits(Flags) } {}
	};

	struct VulkanPipelineStages
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

		struct EXT
		{
			ENUM_BIT(TransformFeedback,				VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT)
			ENUM_BIT(ConditionalRendering,			VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT)
			ENUM_BIT(FragmentDensityProcess,		VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT)
			ENUM_BIT(TaskShader,					VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT)
			ENUM_BIT(MeshShader,					VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT)
			ENUM_BIT(CommandPreprocess,				VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT)
		};

		struct KHR
		{
			ENUM_BIT(AccelerationStructureBuild,	VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR)
			ENUM_BIT(RayTracingShader,				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR)
			ENUM_BIT(FragmentShadingRateAttachment, VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)
		};

		struct NV
		{
			ENUM_BIT(CommandPreprocess,				VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV)
			ENUM_BIT(ShadingRateImage,				VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV)
			ENUM_BIT(RayTracingShader,				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV)
			ENUM_BIT(AccelerationStructureBuild,	VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV)
			ENUM_BIT(TaskShader,					VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV)
			ENUM_BIT(MeshShader,					VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV)
		};

		VkPipelineStageFlagBits Flags;
		VulkanPipelineStages(VkFlags Flags) : Flags{ VkPipelineStageFlagBits(Flags) } {}
	};

} } // namespace VE::Runtime
