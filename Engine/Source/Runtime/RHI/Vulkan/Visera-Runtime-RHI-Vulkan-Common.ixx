module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Common;

export namespace VE
{
	// VE_ENUM_CLASS(EVulkanTransferPipelineStage, VkPipelineStageFlags)
	// VE_ENUM_CLASS(EVulkanComputePipelineStage, VkPipelineStageFlags)
	// VE_ENUM_CLASS(EVulkanFilter,			VkFilter)
	// VE_ENUM_CLASS(EVulkanSharingMode,	VkSharingMode)
	// VE_ENUM_CLASS(EVulkanSwizzle,		VkComponentSwizzle)
	// VE_ENUM_CLASS(EVulkanImageViewType,	VkImageViewType)
	// VE_ENUM_CLASS(EVulkanImageType,		VkImageType)
	// VE_ENUM_CLASS(EVulkanImageAspect,	VkImageAspectFlags)
	// VE_ENUM_CLASS(EVulkanImageTiling,	VkImageTiling)
	// VE_ENUM_CLASS(EVulkanFormat,			VkFormat)
	// VE_ENUM_CLASS(EVulkanPresentMode,	VkPresentModeKHR)
	// VE_ENUM_CLASS(EVulkanDescriptorType,	VkDescriptorType)
	// VE_ENUM_CLASS(EVulkanQueueFamily,	UInt32)
	// VE_ENUM_CLASS(EVulkanCommandPoolType,VkCommandPoolCreateFlags)
	// VE_ENUM_CLASS(EVulkanCommandLevel,	VkCommandBufferLevel)
	// VE_ENUM_CLASS(EVulkanSampleRate,		VkSampleCountFlags)
	// VE_ENUM_CLASS(EVulkanAccess,			VkAccessFlags)
	// VE_ENUM_CLASS(EVulkanShaderStage,	VkShaderStageFlags)
	// VE_ENUM_CLASS(EVulkanGraphicsPipelineStage,	VkPipelineStageFlags)
	// VE_ENUM_CLASS(EVulkanMemoryUsage,	VmaMemoryUsage)
	// VE_ENUM_CLASS(EVulkanImageLayout,	VkImageLayout)
	// VE_ENUM_CLASS(EVulkanImageUsage,		VkImageUsageFlags)
	// VE_ENUM_CLASS(EVulkanBufferUsage,	VkBufferUsageFlags)
	// VE_ENUM_CLASS(EVulkanAttachmentIO,	VkAttachmentLoadOp)

	// struct	FVulkanComponentMapping
	// using	FVulkanExtent2D		= VkExtent2D;
	// using	FVulkanExtent3D		= VkExtent3D;
	// using	FVulkanOffset2D		= VkOffset2D;
	// using	FVulkanOffset3D		= VkOffset3D;
	// using	FVulkanRenderArea	= VkRect2D;
	
	using FClearValue = VkClearValue;
	using FClearColor = VkClearColorValue;

	VE_ENUM_CLASS(EVulkanFilter, VkFilter)
	{
		Nearest = VK_FILTER_NEAREST,
		Linear  = VK_FILTER_LINEAR,
	};

	VE_ENUM_CLASS(EVulkanPipelineBindPoint, VkPipelineBindPoint)
	{
		Graphics = VK_PIPELINE_BIND_POINT_GRAPHICS,
		Compute  = VK_PIPELINE_BIND_POINT_COMPUTE,
	};

	VE_ENUM_CLASS(EVulkanSharingMode, VkSharingMode)
	{
		Exclusive	= VK_SHARING_MODE_EXCLUSIVE,
		Concurrent	= VK_SHARING_MODE_CONCURRENT,
	};
    
	VE_ENUM_CLASS(EVulkanSwizzle, VkComponentSwizzle)
	{
		Identity  = VK_COMPONENT_SWIZZLE_IDENTITY,
		Zero	  = VK_COMPONENT_SWIZZLE_ZERO,
		One		  = VK_COMPONENT_SWIZZLE_ONE,
		R		  = VK_COMPONENT_SWIZZLE_R,
		G	      = VK_COMPONENT_SWIZZLE_G,
		B	      = VK_COMPONENT_SWIZZLE_B,
		A	      = VK_COMPONENT_SWIZZLE_A,
	};

	VE_ENUM_CLASS(EVulkanImageViewType, VkImageViewType)
	{
		Image1D		=	VK_IMAGE_VIEW_TYPE_1D,
		Image2D		= 	VK_IMAGE_VIEW_TYPE_2D,
		Image3D		= 	VK_IMAGE_VIEW_TYPE_3D,
		Cube		= 	VK_IMAGE_VIEW_TYPE_CUBE,
		Array1D		= 	VK_IMAGE_VIEW_TYPE_1D_ARRAY,
		Array2D		= 	VK_IMAGE_VIEW_TYPE_2D_ARRAY,
		ArrayCube	=	VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,

		Auto,
	};

	VE_ENUM_CLASS(EVulkanImageType, VkImageType)
	{
		Image1D = VK_IMAGE_TYPE_1D,
		Image2D = VK_IMAGE_TYPE_2D,
		Image3D = VK_IMAGE_TYPE_3D,

		Undefined,
	};

	VE_ENUM_CLASS(EVulkanImageAspect, VkImageAspectFlags)
	{
		Color		= VK_IMAGE_ASPECT_COLOR_BIT,
		Depth		= VK_IMAGE_ASPECT_DEPTH_BIT,
		Stencil		= VK_IMAGE_ASPECT_STENCIL_BIT,

		Undefined,
	};

	VE_ENUM_CLASS(EVulkanImageTiling, VkImageTiling)
	{
		Optimal = VK_IMAGE_TILING_OPTIMAL,
		Linear	= VK_IMAGE_TILING_LINEAR,
	};

	/* BXXX format is common for Windows bitmap (BGRA) images. */
	VE_ENUM_CLASS(EVulkanFormat, VkFormat)
	{
		None						= VK_FORMAT_UNDEFINED,

		// 8Bits Formats
		U8_Normalized_R4G4			= VK_FORMAT_R4G4_UNORM_PACK8,
		U8_Normalized_R8			= VK_FORMAT_R8_UNORM,
		S8_Normalized_R8			= VK_FORMAT_R8_SNORM,
		U8_Scaled_R8				= VK_FORMAT_R8_USCALED,
		S8_Scaled_R8				= VK_FORMAT_R8_SSCALED,
		U8_Integer_R8				= VK_FORMAT_R8_UINT,
		S8_Integer_R8				= VK_FORMAT_R8_SINT,
		U8_sRGB_R8					= VK_FORMAT_R8_SRGB,

		// 16Bits Formats
		U16_Normalized_R4_G4_B4_A4	= VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		U16_Normalized_B4_G4_R4_A4	= VK_FORMAT_B4G4R4A4_UNORM_PACK16,
		U16_Normalized_R5_G6_B5		= VK_FORMAT_R5G6B5_UNORM_PACK16,
		U16_Normalized_B5_G6_R5		= VK_FORMAT_B5G6R5_UNORM_PACK16,
		U16_Normalized_R5_G5_B5_A1	= VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		U16_Normalized_B5_G5_R5_A1	= VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		U16_Normalized_A1_R5_G5_B5	= VK_FORMAT_A1R5G5B5_UNORM_PACK16,
		U16_Normalized_R8_G8		= VK_FORMAT_R8G8_UNORM,
		S16_Normalized_R8_G8		= VK_FORMAT_R8G8_SNORM,
		U16_Scaled_R8_G8			= VK_FORMAT_R8G8_USCALED,
		S16_Scaled_R8_G8			= VK_FORMAT_R8G8_SSCALED,
		U16_Integer_R8_G8			= VK_FORMAT_R8G8_UINT,
		S16_Integer_R8_G8			= VK_FORMAT_R8G8_SINT,
		U16_sRGB_R8_G8				= VK_FORMAT_R8G8_SRGB,
		
		// 24Bits Formats
		/*VK_FORMAT_R16_UNORM,
		VK_FORMAT_R16_SNORM,
		VK_FORMAT_R16_USCALED,
		VK_FORMAT_R16_SSCALED,
		VK_FORMAT_R16_UINT,
		VK_FORMAT_R16_SINT,
		VK_FORMAT_R16_SFLOAT,
		 VK_FORMAT_R8G8B8_UNORM,
		VK_FORMAT_R8G8B8_SNORM,
		VK_FORMAT_R8G8B8_USCALED,
		VK_FORMAT_R8G8B8_SSCALED,
		VK_FORMAT_R8G8B8_UINT,
		VK_FORMAT_R8G8B8_SINT,
		VK_FORMAT_R8G8B8_SRGB,
		VK_FORMAT_B8G8R8_UNORM,
		VK_FORMAT_B8G8R8_SNORM,
		VK_FORMAT_B8G8R8_USCALED,
		VK_FORMAT_B8G8R8_SSCALED,
		VK_FORMAT_B8G8R8_UINT,
		VK_FORMAT_B8G8R8_SINT,
		VK_FORMAT_B8G8R8_SRGB,*/

		// 32Bits Formats
		U32_Normalized_R8_G8_B8_A8	= VK_FORMAT_R8G8B8A8_UNORM,
		S32_Normalized_R8_G8_B8_A8	= VK_FORMAT_R8G8B8A8_SNORM,
		U32_Scaled_R8_G8_B8_A8		= VK_FORMAT_R8G8B8A8_USCALED,
		S32_Scaled_R8_G8_B8_A8		= VK_FORMAT_R8G8B8A8_SSCALED,
		U32_Integer_R8_G8_B8_A8		= VK_FORMAT_R8G8B8A8_UINT,
		S32_Integer_R8_G8_B8_A8		= VK_FORMAT_R8G8B8A8_SINT,
		U32_sRGB_R8_G8_B8_A8		= VK_FORMAT_R8G8B8A8_SRGB,

		U32_Normalized_B8_G8_R8_A8	= VK_FORMAT_B8G8R8A8_UNORM,
		S32_Normalized_B8_G8_R8_A8	= VK_FORMAT_B8G8R8A8_SNORM,
		U32_Scaled_B8_G8_R8_A8		= VK_FORMAT_B8G8R8A8_USCALED,
		S32_Scaled_B8_G8_R8_A8		= VK_FORMAT_B8G8R8A8_SSCALED,
		U32_Integer_B8_G8_R8_A8		= VK_FORMAT_B8G8R8A8_UINT,
		S32_Integer_B8_G8_R8_A8		= VK_FORMAT_B8G8R8A8_SINT,
		U32_sRGB_B8_G8_R8_A8		= VK_FORMAT_B8G8R8A8_SRGB,

		U32_Normalized_A8_B8_G8_R8	= VK_FORMAT_A8B8G8R8_UNORM_PACK32,
		S32_Normalized_A8_B8_G8_R8	= VK_FORMAT_A8B8G8R8_SNORM_PACK32,
		U32_Scaled_A8_B8_G8_R8		= VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		S32_Scaled_A8_B8_G8_R8		= VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		U32_Integer_A8_B8_G8_R8		= VK_FORMAT_A8B8G8R8_UINT_PACK32,
		S32_Integer_A8_B8_G8_R8		= VK_FORMAT_A8B8G8R8_SINT_PACK32,
		U32_sRGB_A8_B8_G8_R8		= VK_FORMAT_A8B8G8R8_SRGB_PACK32,

		U32_Normalized_A2_R10_G10_B10 = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		S32_Normalized_A2_R10_G10_B10 = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		U32_Scaled_A2_R10_G10_B10	  = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		S32_Scaled_A2_R10_G10_B10	  = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		U32_Integer_A2_R10_G10_B10	  = VK_FORMAT_A2R10G10B10_UINT_PACK32,
		S32_Integer_A2_R10_G10_B10	  = VK_FORMAT_A2R10G10B10_SINT_PACK32,

		U32_Normalized_A2_B10_G10_R10 = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		S32_Normalized_A2_B10_G10_R10 = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		U32_Scaled_A2_B10_G10_R10	  = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		S32_Scaled_A2_B10_G10_R10	  = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		U32_Integer_A2_B10_G10_R10	  = VK_FORMAT_A2B10G10R10_UINT_PACK32,
		S32_Integer_A2_B10_G10_R10	  = VK_FORMAT_A2B10G10R10_SINT_PACK32,

		U32_Normalized_R16_G16	= VK_FORMAT_R16G16_UNORM,
		S32_Normalized_R16_G16	= VK_FORMAT_R16G16_SNORM,
		U32_Scaled_R16_G16		= VK_FORMAT_R16G16_USCALED,
		S32_Scaled_R16_G16		= VK_FORMAT_R16G16_SSCALED,
		U32_Integer_R16_G16		= VK_FORMAT_R16G16_UINT,
		S32_Integer_R16_G16		= VK_FORMAT_R16G16_SINT,
		S32_Float_R16_G16		= VK_FORMAT_R16G16_SFLOAT,

		U32_Integer_R32			= VK_FORMAT_R32_UINT,
		S32_Integer_R32			= VK_FORMAT_R32_SINT,
		S32_Float_R32			= VK_FORMAT_R32_SFLOAT,
		
		S32_Float_Depth32		= VK_FORMAT_D32_SFLOAT,

		// 48Bits Formats
		/*VK_FORMAT_R16G16B16_UNORM,
		VK_FORMAT_R16G16B16_SNORM,
		VK_FORMAT_R16G16B16_USCALED,
		VK_FORMAT_R16G16B16_SSCALED,
		VK_FORMAT_R16G16B16_UINT,
		VK_FORMAT_R16G16B16_SINT,
		VK_FORMAT_R16G16B16_SFLOAT,
		VK_FORMAT_R16G16B16A16_UNORM,
		VK_FORMAT_R16G16B16A16_SNORM,
		VK_FORMAT_R16G16B16A16_USCALED,
		VK_FORMAT_R16G16B16A16_SSCALED,
		VK_FORMAT_R16G16B16A16_UINT,
		VK_FORMAT_R16G16B16A16_SINT,
		VK_FORMAT_R16G16B16A16_SFLOAT,*/

		// 64Bits Formats
		/*VK_FORMAT_R32G32_UINT,
		VK_FORMAT_R32G32_SINT,
		VK_FORMAT_R32G32_SFLOAT,*/

		// 96Bits Formats
		/*VK_FORMAT_R32G32B32_UINT,
		VK_FORMAT_R32G32B32_SINT,
		VK_FORMAT_R32G32B32_SFLOAT,*/
		
		// 128Bits Formats
		/*VK_FORMAT_R32G32B32A32_UINT,
		VK_FORMAT_R32G32B32A32_SINT,
		VK_FORMAT_R32G32B32A32_SFLOAT,*/
	};

	VE_ENUM_CLASS(EVulkanPresentMode, VkPresentModeKHR)
	{
		Immediate	= VK_PRESENT_MODE_IMMEDIATE_KHR,
		Mailbox		= VK_PRESENT_MODE_MAILBOX_KHR,
		FIFO		= VK_PRESENT_MODE_FIFO_KHR,			// V-Sync
		RelaxedFIFO = VK_PRESENT_MODE_FIFO_RELAXED_KHR, // Relaxed V-Sync
	};

	VE_ENUM_CLASS(EVulkanDescriptorType, VkDescriptorType)
	{
		Sampler					= VK_DESCRIPTOR_TYPE_SAMPLER,
		CombinedImageSampler	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,

		SampledImage			= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		StorageImage			= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,

		UniformTexelBuffer		= VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		StorageTexelBuffer		= VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,

		UniformBuffer			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		StorageBuffer			= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		DynamicUniformBuffer	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		DynamicStorageBuffer	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,

		InputAttachment			= VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		InlineUniformBlock		= VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
	};

	VE_ENUM_CLASS(EVulkanQueueFamily, UInt32)
	{
		Graphics = 0,
		Present  = 1,
		Transfer = 2,
		Compute  = 3,
		All		 = 4,
	};

	VE_ENUM_CLASS(EVulkanSampleRate, VkSampleCountFlagBits)
	{
		X1	= VK_SAMPLE_COUNT_1_BIT,
		X2	= VK_SAMPLE_COUNT_2_BIT,
		X4	= VK_SAMPLE_COUNT_4_BIT,
		X8  = VK_SAMPLE_COUNT_8_BIT,
		X16	= VK_SAMPLE_COUNT_16_BIT,
		X32 = VK_SAMPLE_COUNT_32_BIT,
		X64 = VK_SAMPLE_COUNT_64_BIT,
	};
	VE_REGISTER_AUTOCAST(VkSampleCountFlagBits, VkSampleCountFlags);

	VE_ENUM_CLASS(EVulkanCommandPoolType, VkCommandPoolCreateFlags)
	{
		Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		Resetable = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
	};

	VE_ENUM_CLASS(EVulkanCommandLevel, VkCommandBufferLevel)
	{
		Primary   = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
	};

	VE_ENUM_CLASS(EVulkanAccess, VkAccessFlags)
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

	VE_ENUM_CLASS(EVulkanShaderStage,	VkShaderStageFlags)
	{
		Vertex			= VK_SHADER_STAGE_VERTEX_BIT,
		Fragment		= VK_SHADER_STAGE_FRAGMENT_BIT,
		
		Compute			= VK_SHADER_STAGE_COMPUTE_BIT,
	};
	VE_REGISTER_AUTOCAST(VkShaderStageFlags, VkShaderStageFlagBits);

	VE_ENUM_CLASS(EVulkanGraphicsPipelineStage, VkPipelineStageFlags)
	{
		None						= VK_PIPELINE_STAGE_NONE,
		All 						= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,

		PipelineTop					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, // Wait All (Virtual Stage)

		//1. Draw Processing
		//ConditionalRendering		= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT,
		DrawIndirect				= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
		
		//2. Vertex Processing
		VertexInput					= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VertexShader				= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		
		//3. Tessellation
		TessellationControlShader	= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
		TessellationEvaluationShader= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
		
		//4. Primitive Processing
		GeometryShader				= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
		//TransformFeedback			= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT,
		
		//5. Rasterization

		//6. Fragment Processing
		//FragmentShadingRate		= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		EarlyFragmentTests			= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		FragmentShader				= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		LateFragmentTests			= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,

		//7. Pixel Processing
		ColorAttachmentOutput		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	
		PipelineBottom				= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // Wait None (Virtual Stage)

		Transfer					= VK_PIPELINE_STAGE_TRANSFER_BIT,
	};
	
	VE_ENUM_CLASS(EVulkanComputePipelineStage, VkPipelineStageFlags)
	{
		None						= VK_PIPELINE_STAGE_NONE,
		All 						= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,

		PipelineTop					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//ConditionalRendering		= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT,
		DrawIndirect				= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,

		ComputeShader				= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,

		PipelineBottom				= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,

		Transfer					= VK_PIPELINE_STAGE_TRANSFER_BIT,
	};

	VE_ENUM_CLASS(EVulkanTransferPipelineStage, VkPipelineStageFlags)
	{
		None						= VK_PIPELINE_STAGE_NONE,
		All 						= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,

		PipelineTop					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,

		Transfer					= VK_PIPELINE_STAGE_TRANSFER_BIT,

		PipelineBottom				= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
	};

	VE_ENUM_CLASS(EVulkanMemoryUsage,	VmaMemoryUsage)
	{
		None	=	VMA_MEMORY_USAGE_UNKNOWN,
		All		=	VMA_MEMORY_USAGE_MAX_ENUM,
			
		Auto	=	VMA_MEMORY_USAGE_AUTO,
		
		CPU		=	VMA_MEMORY_USAGE_AUTO_PREFER_HOST,

		GPU		=	VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
	};

	VE_ENUM_CLASS(EVulkanImageLayout, VkImageLayout)
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

	VE_ENUM_CLASS(EVulkanImageUsage, VkImageUsageFlags)
	{
		None						= 0x0,
		All							= VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM,

		TransferSource				= VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		TransferDestination			= VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		Sampled						= VK_IMAGE_USAGE_SAMPLED_BIT,
		Storage						= VK_IMAGE_USAGE_STORAGE_BIT,
		ColorAttachment				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		DepthStencilAttachment		= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		TransientAttachment			= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
		InputAttachment				= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
	};

	VE_ENUM_CLASS(EVulkanBufferUsage, VkBufferUsageFlags)
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

	VE_ENUM_CLASS(EVulkanAttachmentIO, VkAttachmentLoadOp)
	{
		I_Keep	 	= VK_ATTACHMENT_LOAD_OP_LOAD,
		I_Clear	 	= VK_ATTACHMENT_LOAD_OP_CLEAR,
		I_Whatever 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,

		O_Store	 	= VK_ATTACHMENT_STORE_OP_STORE,
		O_None	 	= VK_ATTACHMENT_STORE_OP_NONE,
		O_Whatever 	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
	};
	VE_REGISTER_AUTOCAST(VkAttachmentLoadOp, VkAttachmentStoreOp);

	struct FVulkanComponentMapping
	{
		EVulkanSwizzle RMapping = EVulkanSwizzle::Identity;
		EVulkanSwizzle GMapping = EVulkanSwizzle::Identity;
		EVulkanSwizzle BMapping = EVulkanSwizzle::Identity;
		EVulkanSwizzle AMapping = EVulkanSwizzle::Identity;
	};

	using FVulkanExtent2D = VkExtent2D;
	using FVulkanExtent3D = VkExtent3D;

	using FVulkanOffset2D = VkOffset2D;
	using FVulkanOffset3D = VkOffset3D;

	using FVulkanRenderArea	= VkRect2D;

} // namespace VE