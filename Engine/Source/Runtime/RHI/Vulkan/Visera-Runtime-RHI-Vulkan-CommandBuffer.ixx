module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:CommandBuffer;
import :Common;
import :Allocator;
import :Device;
import :RenderPass;
import :Pipeline;
import :DescriptorSet;
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
		
		/*<< State-Type Commands >>*/
		void BindVertexBuffer(SharedPtr<const FVulkanBuffer> _VertexBuffer) const;
		void BindPipeline(SharedPtr<const FVulkanPipeline> _Pipeline) const;
		void PushConstants(const void* _Data, UInt64 _Size) const;
		void SetScissor() const;
		void SetViewport() const;
		//void SetDepthBias() const;
		void BindDescriptorSet(SharedPtr<const FVulkanDescriptorSet> _DescriptorSet) const;

		/*<< Action-Type Commands >>*/
		//Graphics Pipeline Commands
		void Draw(UInt32 _VertexCount, UInt32 _InstanceCount = 1, UInt32 _FirstVertex = 0, UInt32 _FirstInstance = 0) const { vkCmdDraw(Handle, _VertexCount, _InstanceCount, _FirstVertex, _FirstInstance); };
		void BlitImage(SharedPtr<const FVulkanImage> _SrcImage, SharedPtr<FVulkanImage> _DstImage, EVulkanFilter _Filter) const;

		void Reset() { VE_ASSERT(IsIdle() && IsResettable()); vkResetCommandBuffer(Handle, 0x0); WaitStages = 0; CurrentPipeline.reset(); }
		void Free()	 { Status = EStatus::Expired; }

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
		
		auto AddWaitStage(EVulkanGraphicsPipelineStage _WaitStage)		-> FVulkanCommandBuffer* { WaitStages |= AutoCast(_WaitStage); return this; }
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
		SharedPtr<const FVulkanPipeline>	CurrentPipeline;

		EStatus Status = EStatus::Idle;
		Byte bIsResettable  : 1;
		Byte bIsPrimary		: 1;

	public:
		FVulkanCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner, EVulkanCommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};


	void FVulkanCommandBuffer::
	BlitImage(SharedPtr<const FVulkanImage> _SrcImage,
			  SharedPtr<FVulkanImage> _DstImage,
			  EVulkanFilter _Filter) const
	{
		//Check if it is valid usages and layout (Apply Fence before Bliting if necessary.)
		VE_ASSERT(Bool(_SrcImage->GetUsages() & EVulkanImageUsage::TransferSource) &&
				  Bool(_SrcImage->GetLayout() & EVulkanImageLayout::TransferSource));
		VE_ASSERT(Bool(_DstImage->GetUsages() & EVulkanImageUsage::TransferDestination) &&
				  Bool(_DstImage->GetLayout() & EVulkanImageLayout::TransferDestination));
		//vkCmdBlitImage must not be used for multisampled source or destination images. Use vkCmdResolveImage for this purpose.
		VE_ASSERT(!_SrcImage->EnabledMSAA() && !_DstImage->EnabledMSAA());

		auto& SrcExtent = _SrcImage->GetExtent();
		auto& DstExtent = _DstImage->GetExtent();
		VkImageBlit BlitInfo
		{
			.srcSubresource
			{
				.aspectMask = AutoCast(_SrcImage->GetAspects()),
				.mipLevel	= _SrcImage->GetMipmapLevels(),
				.baseArrayLayer = 0,
				.layerCount = _SrcImage->GetArrayLayers(),
			},
			.srcOffsets
			{
				{0, 0, 0},
				{Int32(SrcExtent.width), Int32(SrcExtent.height), Int32(SrcExtent.depth)},
			},
			.dstSubresource
			{
				.aspectMask = AutoCast(_DstImage->GetAspects()),
				.mipLevel	= _DstImage->GetMipmapLevels(),
				.baseArrayLayer = 0,
				.layerCount = _DstImage->GetArrayLayers(),
			},
			.dstOffsets
			{
				{0, 0, 0},
				{Int32(DstExtent.width), Int32(DstExtent.height), Int32(DstExtent.depth)},
			}
		};

		vkCmdBlitImage(
			Handle,
			_SrcImage->GetHandle(),
			AutoCast(_SrcImage->GetLayout()),
			_DstImage->GetHandle(),
			AutoCast(_DstImage->GetLayout()),
			1,
			&BlitInfo,
			AutoCast(_Filter));
	}

	void FVulkanCommandBuffer::
	PushConstants(const void* _Data, UInt64 _Size) const
	{
		if (CurrentPipeline != nullptr)
		{
			VE_ASSERT(!CurrentPipeline->GetLayout()->GetPushConstantRanges().empty());
			vkCmdPushConstants(
				Handle,
				CurrentPipeline->GetLayout()->GetHandle(),
				WaitStages,
				0,
				_Size,
				_Data
			);
		}
		else throw SRuntimeError("Call BindPipeline(...) before PushConstants(...)!");
	}

	void FVulkanCommandBuffer::
	SetScissor() const
	{
		vkCmdSetScissorWithCount(Handle, CurrentScissors.size(), CurrentScissors.data());
	}

	void FVulkanCommandBuffer::
	SetViewport() const
	{
		vkCmdSetViewportWithCount(Handle, CurrentViewports.size(), CurrentViewports.data());
	}

	void FVulkanCommandBuffer::
	BindDescriptorSet(SharedPtr<const FVulkanDescriptorSet> _DescriptorSet) const
	{
		auto DescriptorSetHandle = _DescriptorSet->GetHandle();
		if (CurrentPipeline != nullptr)
		{
			vkCmdBindDescriptorSets(Handle,
				AutoCast(CurrentPipeline->GetBindPoint()),
				CurrentPipeline->GetLayout()->GetHandle(),
				0,
				1,
				&DescriptorSetHandle,
				0,
				nullptr);
		}
		else throw SRuntimeError("Call BindPipeline(...) before BindDescriptorSet(...)!");
	}

	void FVulkanCommandBuffer::
	BindVertexBuffer(SharedPtr<const FVulkanBuffer> _VertexBuffer) const
	{
		auto VertexBufferHanedle = _VertexBuffer->GetHandle();
		vkCmdBindVertexBuffers(Handle, 0, 1, &VertexBufferHanedle, nullptr);
	}

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

		//Implicitly Reset Current CommandBuffer.
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