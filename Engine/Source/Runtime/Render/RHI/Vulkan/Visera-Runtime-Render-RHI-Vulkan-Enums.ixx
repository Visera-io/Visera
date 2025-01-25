module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Enums;

export namespace VE { namespace Runtime
{


//VE_ENUM_CLASS(EQueueFamily, UInt32)
//VE_ENUM_CLASS(ECommandPool, VkCommandPoolCreateFlags)
//VE_ENUM_CLASS(ESampleRate, VkSampleCountFlags)
//VE_ENUM_CLASS(EAccessibility,	VkAccessFlags)
//VE_ENUM_CLASS(EShaderStage,	VkShaderStageFlags)
//VE_ENUM_CLASS(EPipelineStage,	VkPipelineStageFlags)
//VE_ENUM_CLASS(EMemoryUsage,	VmaMemoryUsage)
//VE_ENUM_CLASS(EImageLayout,	VkImageLayout)
//VE_ENUM_CLASS(EImageUsage,	VkImageUsageFlags)
//VE_ENUM_CLASS(EBufferUsage,	VkBufferUsageFlags)
//VE_ENUM_CLASS(EAttachmentIO,	VkAttachmentLoadOp)

	VE_ENUM_CLASS(EQueueFamily, UInt32)
	{
		Graphics = 0,
		Present  = 1,
		Transfer = 2,
		Compute  = 3,
		All		 = 4,
	};

	VE_ENUM_CLASS(ESampleRate, VkSampleCountFlags)
	{
		X1	= VK_SAMPLE_COUNT_1_BIT,
		X2	= VK_SAMPLE_COUNT_2_BIT,
		X4	= VK_SAMPLE_COUNT_4_BIT,
		X8  = VK_SAMPLE_COUNT_8_BIT,
		X16	= VK_SAMPLE_COUNT_16_BIT,
		X32 = VK_SAMPLE_COUNT_32_BIT,
		X64 = VK_SAMPLE_COUNT_64_BIT,
	};
	VE_REGISTER_AUTOCAST(VkSampleCountFlags, VkSampleCountFlagBits);

	VE_ENUM_CLASS(ECommandPool, VkCommandPoolCreateFlags)
	{
		Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		Resetable = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
	};


	VE_ENUM_CLASS(EAccessibility, VkAccessFlags)
	{
		None						= VK_ACCESS_NONE,

		R_IndirectCommand			= VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
		R_Index						= VK_ACCESS_INDEX_READ_BIT,
		R_VertexAttribute			= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
		R_Uniform					= VK_ACCESS_UNIFORM_READ_BIT,
		R_InputAttachment			= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
		R_Shader					= VK_ACCESS_SHADER_READ_BIT,
		R_ColorAttachment			= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		R_DepthStencilAttachment	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		R_Transfer					= VK_ACCESS_TRANSFER_READ_BIT,
		R_Host						= VK_ACCESS_HOST_READ_BIT,
		R_Memory					= VK_ACCESS_MEMORY_READ_BIT,

		W_Shader					= VK_ACCESS_SHADER_WRITE_BIT,
		W_ColorAttachment			= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		W_DepthStencilAttachment	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		W_Transfer					= VK_ACCESS_TRANSFER_WRITE_BIT,
		W_Host						= VK_ACCESS_HOST_WRITE_BIT,
		W_Memory					= VK_ACCESS_MEMORY_WRITE_BIT,

		X_Shader				= R_Shader					| W_Shader,
		X_ColorAttachment		= R_ColorAttachment			| W_ColorAttachment,
		X_DepthStencilAttachment= R_DepthStencilAttachment	| W_DepthStencilAttachment,
		X_Transfer				= R_Transfer				| W_Transfer,
		X_Host					= R_Host					| W_Host,
		X_Memory				= R_Memory					| W_Memory,
	};

	VE_ENUM_CLASS(EShaderStage,	VkShaderStageFlags)
	{
		Vertex			= VK_SHADER_STAGE_VERTEX_BIT,
		Fragment		= VK_SHADER_STAGE_FRAGMENT_BIT,
	};

	VE_ENUM_CLASS(EPipelineStage, VkPipelineStageFlags)
	{
		None							= VK_PIPELINE_STAGE_NONE,
		All								= VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM,

		PipelineTop						= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		DrawIndirect					= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
		VertexInput						= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VertexShader					= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		TessellationControlShader		= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
		TessellationEvaluationShader	= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
		GeometryShader					= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
		FragmentShader					= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		EarlyFragmentTests				= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		LateFragmentTests				= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		ColorAttachmentOutput			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		ComputeShader					= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		Transfer						= VK_PIPELINE_STAGE_TRANSFER_BIT,
		PipelineBottom					= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,

		Host 	= VK_PIPELINE_STAGE_HOST_BIT,
		AllGraphics 	= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		AllCommands 	= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	};

	VE_ENUM_CLASS(EMemoryUsage,	VmaMemoryUsage)
	{
		None	=	VMA_MEMORY_USAGE_UNKNOWN,
		All		=	VMA_MEMORY_USAGE_MAX_ENUM,
			
		Auto	=	VMA_MEMORY_USAGE_AUTO,
		
		CPU		=	VMA_MEMORY_USAGE_AUTO_PREFER_HOST,

		GPU		=	VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};

	VE_ENUM_CLASS(EImageLayout, VkImageLayout)
	{
		Undefined 							= VK_IMAGE_LAYOUT_UNDEFINED,
		General 							= VK_IMAGE_LAYOUT_GENERAL,

		ColorAttachment						= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		DepthStencilAttachment 				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		DepthStencilReadOnly 				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		ShaderReadOnly 						= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		TransferSource 						= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		TransferDestination 				= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		Preinitialized						= VK_IMAGE_LAYOUT_PREINITIALIZED,
		DepthReadOnlyStencilAttachment		= VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
		DepthAttachmentStencilReadOnly		= VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
		DepthAttachment						= VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		DepthReadOnly						= VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
		StencilAttachment					= VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
		StencilReadOnly						= VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
		ReadOnly							= VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
		Attachment							= VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,

		Present								= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	VE_ENUM_CLASS(EImageUsage, VkImageUsageFlags)
	{
		None	= 0x0,
		All			= VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM,

		TransferSource				= VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		TransferDestination			= VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		Sampled						= VK_IMAGE_USAGE_SAMPLED_BIT,
		Storage						= VK_IMAGE_USAGE_STORAGE_BIT,
		ColorAttachment				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		DepthStencilAttachment		= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		TransientAttachment			= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
		InputAttachment				= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
	};

	VE_ENUM_CLASS(EBufferUsage, VkBufferUsageFlags)
	{
		None	= 0x0,
		All		= VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM,

		Uniform				= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		UniformTexel		= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
		Storage				= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		StorageTexel		= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
		Index				= VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		TransferSource 		= VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		TransferDestination	= VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			
		Indirect			= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		SharedDeviceAddress	= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
	};

	VE_ENUM_CLASS(EAttachmentIO, VkAttachmentLoadOp)
	{
		I_Keep	 	= VK_ATTACHMENT_LOAD_OP_LOAD,
		I_Clear	 	= VK_ATTACHMENT_LOAD_OP_CLEAR,
		I_Whatever 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,

		O_Store	 	= VK_ATTACHMENT_STORE_OP_STORE,
		O_None	 	= VK_ATTACHMENT_STORE_OP_NONE,
		O_Whatever 	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
	};

} } // namespace VE::Runtime