module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:CommandBuffer;
import :Common;
import :Device;
import :RenderPass;
import :Pipeline;
import :Synchronization;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{ 
	class RHI;
	class FVulkanCommandPool;

	class FVulkanCommandBuffer : public std::enable_shared_from_this<FVulkanCommandBuffer>
	{
		friend class RHI;
		friend class FVulkanCommandPool;
	public:
		enum class EStatus { Expired, Idle, Recording, InsideRenderPass, Submitted };

		void StartRecording();
		void StopRecording();
		void StartRenderPass(SharedPtr<FVulkanRenderPass> _RenderPass);
		void StopRenderPass()	  { VE_ASSERT(IsInsideRenderPass()); vkCmdEndRenderPass(Handle); Status = EStatus::Recording; }
		
		void BindPipeline(SharedPtr<const FVulkanPipeline> _Pipeline) const;
		void Draw(UInt32 _VertexCount, UInt32 _InstanceCount = 1, UInt32 _FirstVertex = 0, UInt32 _FirstInstance = 0) const { vkCmdDraw(Handle, _VertexCount, _InstanceCount, _FirstVertex, _FirstInstance); };

		void Reset()			  { VE_ASSERT(IsIdle() && IsResettable()); vkResetCommandBuffer(Handle, 0x0); WaitStages = 0; }
		void Free()				  { Status = EStatus::Expired; }

		Bool IsExpired()			const { return Status == EStatus::Expired;			}
		Bool IsSubmitted()			const { return Status == EStatus::Submitted;		}
		Bool IsIdle()				const { return Status == EStatus::Idle;				}
		Bool IsRecording()			const { return Status == EStatus::Recording;		}
		Bool IsInsideRenderPass()	const { return Status == EStatus::InsideRenderPass; }
		Bool IsResettable()			const { return bIsResettable;   }
		Bool IsPrimary()			const { return bIsPrimary;		}

		auto GetLevel()				const -> EVulkanCommandLevel	{ return IsPrimary()? EVulkanCommandLevel::Primary : EVulkanCommandLevel::Secondary; }
		auto GetSubmitInfo()		const -> const VkSubmitInfo;
		auto GetHandle()			const -> const VkCommandBuffer	{ return Handle; }
		
		auto AddWaitStage(EVulkanPipelineStage _WaitStage)		-> FVulkanCommandBuffer* { WaitStages |= AutoCast(_WaitStage); return this; }
		auto AddWaitSemaphore(const VkSemaphore _WaitSemaphore)		-> FVulkanCommandBuffer* { WaitSemaphoreViews.push_back(_WaitSemaphore); return this; }
		auto AddSignalSemaphore(const VkSemaphore _SignalSemaphore)	-> FVulkanCommandBuffer* { SignalSemaphoreViews.push_back(_SignalSemaphore); return this; }

	private:
		VkCommandBuffer						Handle{ VK_NULL_HANDLE };
		WeakPtr<const FVulkanCommandPool>   Owner;
		Array<VkSemaphore>					WaitSemaphoreViews;
		VkPipelineStageFlags				WaitStages{ 0 };
		Array<VkSemaphore>					SignalSemaphoreViews;
		Segment<VkViewport,1>				CurrentViewports{ VkViewport{.minDepth = 0.0f, .maxDepth = 1.0f} };
		Segment<VkRect2D, 1>				CurrentScissors{ VkRect2D {.offset{.x = 0, .y = 0}, .extent{.width = 0, .height = 0} } };

		EStatus Status = EStatus::Idle;
		Byte bIsResettable  : 1;
		Byte bIsPrimary		: 1;

	public:
		FVulkanCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner, EVulkanCommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};

	const VkSubmitInfo FVulkanCommandBuffer::
	GetSubmitInfo() const
	{
		return VkSubmitInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext					= nullptr,
			.waitSemaphoreCount		= UInt32(WaitSemaphoreViews.size()),
			.pWaitSemaphores		= WaitSemaphoreViews.data(),
			.pWaitDstStageMask		= &WaitStages,
			.commandBufferCount		= 1,
			.pCommandBuffers		= &Handle,
			.signalSemaphoreCount	= UInt32(SignalSemaphoreViews.size()),
			.pSignalSemaphores		= SignalSemaphoreViews.data(),
		};
	}

	void FVulkanCommandBuffer::
	StartRenderPass(SharedPtr<FVulkanRenderPass> _RenderPass)
	{ 
		VE_ASSERT(IsRecording());

		static VkRenderPassBeginInfo RenderPassBeginInfo{};
		RenderPassBeginInfo = std::move(_RenderPass->GetRenderPassBeginInfo());

		vkCmdBeginRenderPass(
			Handle,
			&RenderPassBeginInfo,
			IsPrimary()? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		const auto& RenderArea = _RenderPass->GetRenderArea();
		CurrentViewports[0].x = RenderArea.offset.x;
		CurrentViewports[0].y = RenderArea.offset.y;
		CurrentViewports[0].width  = RenderArea.extent.width;
		CurrentViewports[0].height = RenderArea.extent.height;

		Status = EStatus::InsideRenderPass;
	}

	void FVulkanCommandBuffer::
	BindPipeline(SharedPtr<const FVulkanPipeline> _Pipeline) const
	{
		VE_ASSERT(IsInsideRenderPass());
		vkCmdBindPipeline(Handle, AutoCast(_Pipeline->GetBindPoint()), _Pipeline->GetHandle());

		vkCmdSetViewportWithCount(Handle, CurrentViewports.size(), CurrentViewports.data());
		vkCmdSetScissorWithCount(Handle, CurrentScissors.size(), CurrentScissors.data());
	}
	
	FVulkanCommandBuffer::
	FVulkanCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner, EVulkanCommandLevel _Level) noexcept
		: 
		Owner { _Owner },
		bIsPrimary { EVulkanCommandLevel::Primary == _Level? True : False }
	{
		
	}

	FVulkanCommandBuffer::
	~FVulkanCommandBuffer() noexcept
	{
		Status = EStatus::Expired; // Collected by Owner Pool
	}

	void FVulkanCommandBuffer::
	StartRecording()
	{
		VE_ASSERT(!IsRecording() && !IsExpired());

		VkCommandBufferBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (VK_SUCCESS != vkBeginCommandBuffer(Handle, &BeginInfo))
		{ throw SRuntimeError("Failed to begin recording Vulkan Command FVulkanBuffer!"); }

		Status = EStatus::Recording;
	}

	void FVulkanCommandBuffer::
	StopRecording()
	{
		VE_ASSERT(IsRecording());

		if (VK_SUCCESS != vkEndCommandBuffer(Handle))
		{ throw SRuntimeError("Failed to stop recording Vulkan Command FVulkanBuffer!"); }

		Status = EStatus::Idle;
	}
	
} } // namespace VE::Runtime