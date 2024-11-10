module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:RenderPass;

import :Context;
import :Allocator;
import :Device;
import :Shader;
import :CommandPool;
import :Swapchain;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanRenderPass
	{
		friend class Vulkan;
		class Subpass;
	public:
		virtual void Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const;
		virtual void Stop(SharedPtr<VulkanCommandPool::CommandBuffer>  CommandBuffer) const;
		auto GetSubpasses() const -> const Array<Subpass>& { return Subpasses; }

	public:
		class Subpass
		{
		public:
			auto GetViewport() const ->	const VkViewport&	{ return Viewport; }
			auto GetScissor() const ->	const VkRect2D&		{ return Scissor; }
			auto GetColorBlendAttachmentState() const -> const VkPipelineColorBlendAttachmentState& { return ColorBlendAttachmentState; }

		private:
			VkPipelineVertexInputStateCreateInfo	VertexInputState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
			VkPipelineTessellationStateCreateInfo	TessellationState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
			VkPipelineInputAssemblyStateCreateInfo	InputAssemblyState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
			VkPipelineViewportStateCreateInfo		ViewportState		{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
			VkPipelineRasterizationStateCreateInfo	RasterizationState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
			VkPipelineMultisampleStateCreateInfo	MultisampleState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
			VkPipelineDepthStencilStateCreateInfo	DepthStencilState	{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
			VkPipelineColorBlendStateCreateInfo		ColorBlendState		{ .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
			VkPipelineDynamicStateCreateInfo		DynamicState		{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

			SharedPtr<VulkanShader>				VertexShader;
			SharedPtr<VulkanShader>				FragmentShader;
			VkViewport							Viewport;
			VkRect2D							Scissor;
			VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
		};

	protected:
		struct FrameBuffer
		{
			VkFramebuffer		Handle{ VK_NULL_HANDLE };
			Array<VkImageView>	RenderTargets;
			Array<VkClearValue>	ClearColors;
			operator VkFramebuffer() const { return Handle; }
			~FrameBuffer() { vkDestroyFramebuffer(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks); Handle = VK_NULL_HANDLE; }
		};
		VkRenderPass					Handle{ VK_NULL_HANDLE };
		UInt32							Priority { 0 }; // Less is More
		VkRect2D						RenderArea; //Default: {{0,0}, {Swapchain.Extent}}
		Array<FrameBuffer>				FrameBuffers;
		Array<VkAttachmentDescription>  Attachments;
		Array<Subpass>					Subpasses;

	public:
		VulkanRenderPass() noexcept;
		virtual ~VulkanRenderPass() noexcept;
	};

	void VulkanRenderPass::
	Start(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const
	{
		Assert(CommandBuffer->IsRecording());

		auto& CurrentFrameBuffer = FrameBuffers[GVulkan->Swapchain->GetCursor()];
		VkRenderPassBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass		= Handle,
			.framebuffer	= CurrentFrameBuffer.Handle,
			.renderArea		= RenderArea,
			.clearValueCount= UInt32(CurrentFrameBuffer.ClearColors.size()),
			.pClearValues	= CurrentFrameBuffer.ClearColors.data()
		};
		VkSubpassContents SubpassContents =
			CommandBuffer->IsPrimary() ?
			VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

		vkCmdBeginRenderPass(CommandBuffer->GetHandle(), &BeginInfo, SubpassContents);
	}

	void VulkanRenderPass::
	Stop(SharedPtr<VulkanCommandPool::CommandBuffer> CommandBuffer) const
	{
		Assert(CommandBuffer->IsRecording());
		vkCmdEndRenderPass(CommandBuffer->GetHandle());
	}

	VulkanRenderPass::
	VulkanRenderPass() noexcept
		:RenderArea{{0,0}, { GVulkan->Swapchain->GetExtent() }}
	{
		FrameBuffers.resize(GVulkan->Swapchain->GetImages().size());
	}

	VulkanRenderPass::
	~VulkanRenderPass() noexcept
	{
		Subpasses.clear();
		FrameBuffers.clear();
		vkDestroyRenderPass(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
	
} } // namespace VE::Runtime