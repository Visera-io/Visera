module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Common;

export namespace VE { namespace Runtime
{
    #define Flag = Bit, static constexpr VkFlags Flag = Bit;

	struct VulkanAccessPermissions;
	struct VulkanShaderStages;
	struct VulkanPipelineStages;
	struct VulkanImageLayouts;
	struct VulkanImageUsages;
	struct VulkanAttachmentIO;

    struct VulkanAccessPermissions
    {
		enum Option : VkFlags
		{
			None					= VK_ACCESS_NONE,

            R_IndirectCommand		= VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
            R_Index					= VK_ACCESS_INDEX_READ_BIT,
            R_VertexAttribute		= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            R_Uniform				= VK_ACCESS_UNIFORM_READ_BIT,
            R_InputAttachment		= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            R_Shader				= VK_ACCESS_SHADER_READ_BIT,
            R_ColorAttachment		= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
            R_DepthStencilAttachment= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            R_Transfer				= VK_ACCESS_TRANSFER_READ_BIT,
            R_Host					= VK_ACCESS_HOST_READ_BIT,
            R_Memory				= VK_ACCESS_MEMORY_READ_BIT,

            W_Shader				= VK_ACCESS_SHADER_WRITE_BIT,
            W_ColorAttachment		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            W_DepthStencilAttachment= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            W_Transfer				= VK_ACCESS_TRANSFER_WRITE_BIT,
            W_Host					= VK_ACCESS_HOST_WRITE_BIT,
            W_Memory				= VK_ACCESS_MEMORY_WRITE_BIT,

			X_Shader				= R_Shader					| W_Shader,
			X_ColorAttachment		= R_ColorAttachment			| W_ColorAttachment,
			X_DepthStencilAttachment= R_DepthStencilAttachment	| W_DepthStencilAttachment,
			X_Transfer				= R_Transfer				| W_Transfer,
			X_Host					= R_Host					| W_Host,
			X_Memory				= R_Memory					| W_Memory,
		};
    };

    struct VulkanShaderStages
	{
		enum Option : VkFlags
		{
			Vertex		= VK_SHADER_STAGE_VERTEX_BIT,
			Fragment	= VK_SHADER_STAGE_FRAGMENT_BIT,
		};
	};

	struct VulkanPipelineStages
	{
		enum Option : VkFlags
		{
			None						= VK_PIPELINE_STAGE_NONE,
			All							= VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM,
			PipelineTop					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			DrawIndirect				= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
			VertexInput					= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VertexShader				= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			TessellationControlShader	= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
			TessellationEvaluationShader= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
			GeometryShader				= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
			FragmentShader				= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			EarlyFragmentTests			= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			LateFragmentTests			= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			ColorAttachmentOutput		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			ComputeShader				= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			Transfer					= VK_PIPELINE_STAGE_TRANSFER_BIT,
			PipelineBottom				= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,

			Host = VK_PIPELINE_STAGE_HOST_BIT,
			AllGraphics = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			AllCommands = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		};

		struct EXT { enum Option : VkFlags
		{
				TransformFeedback		= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT,
				ConditionalRendering	= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT,
				FragmentDensityProcess	= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT,
				TaskShader				= VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT,
				MeshShader				= VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT,
				CommandPreprocess		= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT,
		}; };

		struct KHR { enum Option : VkFlags
		{
			AccelerationStructureBuild		= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			RayTracingShader				= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
			FragmentShadingRateAttachment	= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		}; };

		struct NV { enum Option : VkFlags
		{
			CommandPreprocess			= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV,
			ShadingRateImage			= VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV,
			RayTracingShader			= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV,
			AccelerationStructureBuild	= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
			TaskShader					= VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV,
			MeshShader					= VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV,
		}; };
	};

	struct VulkanImageLayouts
	{
		enum Option : VkFlags
		{
			Undefined 						= VK_IMAGE_LAYOUT_UNDEFINED,
			General 						= VK_IMAGE_LAYOUT_GENERAL,
			ColorAttachment					= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			DepthStencilAttachment 			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			DepthStencilReadOnly 			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			ShaderReadOnly 					= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			TransferSource 					= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			TransferDestination 			= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			Preinitialized					= VK_IMAGE_LAYOUT_PREINITIALIZED,
			DepthReadOnlyStencilAttachment	= VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
			DepthAttachmentStencilReadOnly	= VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
			DepthAttachment					= VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			DepthReadOnly					= VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
			StencilAttachment				= VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
			StencilReadOnly					= VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
			ReadOnly						= VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
			Attachment						= VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,

			Present							= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		};
	};

	struct VulkanImageUsages
	{
		enum Option : VkFlags
		{
			None	= 0x0,
			All		= VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM,

			TransferSource			= VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			TransferDestination		= VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			Sampled					= VK_IMAGE_USAGE_SAMPLED_BIT,
			Storage					= VK_IMAGE_USAGE_STORAGE_BIT,
			ColorAttachment			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			DepthStencilAttachment	= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			TransientAttachment		= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
			InputAttachment			= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			//VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
			//VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR,
			//VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR,
			/*VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT,
			VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
			VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT,
			VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR,
			VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR,
			VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR,
			VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT,
			VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI,
			VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM,
			VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM,
			VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV,*/
		};
	};

	struct VulkanAttachmentIO //VkAttachmentStoreOp;
	{
		enum Option : VkFlags
		{
			I_Keep	 = VK_ATTACHMENT_LOAD_OP_LOAD,
			I_Clear	 = VK_ATTACHMENT_LOAD_OP_CLEAR,
			I_Whatever = VK_ATTACHMENT_LOAD_OP_DONT_CARE,

			O_Store	 = VK_ATTACHMENT_STORE_OP_STORE,
			O_None	 = VK_ATTACHMENT_STORE_OP_NONE,
			O_Whatever = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		};
	};

} } // namespace VE::Runtime
