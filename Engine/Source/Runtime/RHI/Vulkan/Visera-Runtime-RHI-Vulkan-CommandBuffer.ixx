module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:CommandBuffer;

import :Enums;
import :Device;
import :RenderPass;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{ 
	class FVulkanCommandPool;

	class FVulkanCommandBuffer
	{
		friend class FVulkanCommandPool;
	public:
		enum class EStatus { Idle, Recording, InsideRenderPass };

		void StartRecording();
		void StopRecording();
		void StartRenderPass(SharedPtr<FVulkanRenderPass> _RenderPass);
		void StopRenderPass()	  { VE_ASSERT(IsInsideRenderPass()); vkCmdEndRenderPass(Handle); Status = EStatus::Recording; }
		void Reset()			  { VE_ASSERT(IsIdle() && IsResettable()); vkResetCommandBuffer(Handle, 0x0); }

		Bool IsIdle()				const { return Status == EStatus::Idle; }
		Bool IsRecording()			const { return Status >= EStatus::Recording; }
		Bool IsInsideRenderPass()	const { return Status >= EStatus::InsideRenderPass; }
		Bool IsResettable()			const { return bIsResettable;   }
		Bool IsPrimary()			const { return bIsPrimary;		}
		auto GetLevel()				const -> ECommandLevel { return IsPrimary()? ECommandLevel::Primary : ECommandLevel::Secondary; }
		auto GetHandle()				  -> VkCommandBuffer		{ return Handle; }
		auto GetOwner()				const -> const VkCommandPool	{ return Owner; }
		operator VkCommandBuffer() { return Handle; }

	private:
		VkCommandBuffer				  Handle{ VK_NULL_HANDLE };
		VkCommandPool				  Owner { VK_NULL_HANDLE };
		Array<VkViewport>			  CurrentViewports;
		Array<VkRect2D>				  CurrentScissors;

		EStatus Status = EStatus::Idle;
		Byte bIsResettable  : 1;
		Byte bIsPrimary		: 1;

	public:
		FVulkanCommandBuffer(VkCommandPool _Owner, ECommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};

	void FVulkanCommandBuffer::
	StartRenderPass(SharedPtr<FVulkanRenderPass> _RenderPass)
	{ 
		VE_ASSERT(!IsInsideRenderPass());
		
		VE_WIP;//Framebuffer
		VkRenderPassBeginInfo RenderPassBeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = _RenderPass->GetHandle(),
			//.framebuffer = ,
			//.renderArea = _RenderPass->GetRenderArea(),
			//.clearValueCount = _RenderPass->Get
			//.pClearValues
		};
		vkCmdBeginRenderPass(
			Handle,
			&RenderPassBeginInfo,
			IsPrimary()? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		Status = EStatus::InsideRenderPass;
	}

	
	FVulkanCommandBuffer::
	FVulkanCommandBuffer(VkCommandPool _Owner, ECommandLevel _Level) noexcept
		: Owner { _Owner },
		  bIsPrimary { ECommandLevel::Primary == _Level? True : False }
	{
		VkCommandBufferAllocateInfo AllocateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool	= Owner,
			.level			= AutoCast(GetLevel()),
			.commandBufferCount = 1
		};

		if(vkAllocateCommandBuffers(
			GVulkan->Device->GetHandle(),
			&AllocateInfo,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create FCommandBuffer!"); }
	}

	FVulkanCommandBuffer::
	~FVulkanCommandBuffer() noexcept
	{
		vkFreeCommandBuffers(GVulkan->Device->GetHandle(), Owner, 1, &Handle);
		Handle = VK_NULL_HANDLE;
	}

	void FVulkanCommandBuffer::
	StartRecording()
	{
		VE_ASSERT(!IsRecording());

		VkCommandBufferBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (VK_SUCCESS != vkBeginCommandBuffer(Handle, &BeginInfo))
		{ throw SRuntimeError("Failed to begin recording Vulkan Command FBuffer!"); }

		Status = EStatus::Recording;
	}

	void FVulkanCommandBuffer::
	StopRecording()
	{
		VE_ASSERT(IsRecording());

		if (VK_SUCCESS != vkEndCommandBuffer(Handle))
		{ throw SRuntimeError("Failed to stop recording Vulkan Command FBuffer!"); }

		Status = EStatus::Idle;
	}
	
} } // namespace VE::Runtime