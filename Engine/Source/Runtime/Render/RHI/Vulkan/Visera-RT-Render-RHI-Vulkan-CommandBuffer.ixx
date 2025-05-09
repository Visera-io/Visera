module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:CommandBuffer;
#define VE_MODULE_NAME "Vulkan:CommandBuffer"
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Allocator;
import :Device;
import :RenderPass;
import :Pipeline;
import :RenderPipeline;
import :DescriptorSet;
import :Synchronization;

import Visera.Core.Log;

export namespace VE
{
	class FVulkanCommandPool;
	class FVulkanGraphicsCommandPool;

	class FVulkanCommandBuffer : public std::enable_shared_from_this<FVulkanCommandBuffer>
	{
		friend class FVulkanCommandPool;
	public:
		enum class EStatus { Expired, Idle, Recording, ReadyToSubmit, Submitted }; //[FIXME]: Currrent Status is not trustable
		
		void StartRecording();
		void StopRecording();
		void Reset() const { VE_ASSERT(IsIdle() && IsResettable()); vkResetCommandBuffer(Handle, 0x0); }

		Bool IsExpired()			const { return Status == EStatus::Expired;		}
		Bool IsIdle()				const { return Status == EStatus::Idle;			}
		Bool IsRecording()			const { return Status == EStatus::Recording;	}
		Bool IsReadyToSubmit()		const { return Status == EStatus::ReadyToSubmit;}
		Bool IsSubmitted()			const { return Status == EStatus::Submitted;	}
		Bool IsResettable()			const { return bIsResettable;   }
		Bool IsPrimary()			const { return bIsPrimary;		}

		void WriteBuffer(SharedPtr<FVulkanBuffer> _Destination, UInt64 _DestOffset, SharedPtr<const FVulkanBuffer> _Source, UInt64 _SrcOffset, UInt64 _Size);
		void WriteBuffer(SharedPtr<FVulkanBuffer> _Destination, SharedPtr<const FVulkanBuffer> _Source, UInt64 _Size = 0);

		auto GetStatus()			const -> EStatus				{ return Status; }
		auto GetLevel()				const -> EVulkanCommandLevel	{ return IsPrimary()? EVulkanCommandLevel::Primary : EVulkanCommandLevel::Secondary; }
		auto GetHandle()			const -> const VkCommandBuffer	{ return Handle; }

		void SetStatus(EStatus _NewStatus) { Status = _NewStatus; }

		void BindPipeline(SharedPtr<const FVulkanPipeline> _Pipeline);
		void PushConstants(EVulkanShaderStage _ShaderStages, const void* _Data, UInt32 _Size, UInt32 _Offset);
		void BindDescriptorSet(UInt32 _Index, SharedPtr<const FVulkanDescriptorSet> _DescriptorSet);

		void Free()	 { Status = EStatus::Expired; }

	protected:
		VkCommandBuffer						Handle{ VK_NULL_HANDLE };
		WeakPtr<const FVulkanCommandPool>   Owner;
		EVulkanQueueFamily					QueueFamily;

		SharedPtr<const FVulkanPipeline>	CurrentPipeline;

		mutable EStatus Status = EStatus::Idle;
		UInt8 bIsResettable  : 1;
		UInt8 bIsPrimary	 : 1;

	public:
		FVulkanCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner, EVulkanQueueFamily _QueueFamily, EVulkanCommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};

	class FVulkanGraphicsCommandBuffer : public FVulkanCommandBuffer
	{
		friend class FVulkanGraphicsCommandPool;
	public:
		void StartRecording() { FVulkanCommandBuffer::StartRecording(); }
		void StopRecording()  { FVulkanCommandBuffer::StopRecording();  }
		
		void Submit(const VkSemaphore*                  _pWaitSemaphores,   UInt32 _WaitSemaphoreCount,
			        const EVulkanGraphicsPipelineStage* _pWaitStages,    // WaitStageCount == _WaitSemaphoreCount
			        const VkSemaphore*                  _pSignalSemaphores, UInt32 _SignalSemaphoreCount,
			        const FVulkanFence*                 _SignalFence = nullptr) const;
		void SubmitAndWait() const { FVulkanFence Fence{}; Submit(nullptr, 0, nullptr, nullptr, 0, &Fence); Fence.Wait(); }

		void WriteImage(SharedPtr<FVulkanImage> _Image, SharedPtr<const FVulkanBuffer> _StagingBuffer);

		void ReachRenderPass(SharedPtr<const FVulkanRenderPass> _RenderPass);
		void BindVertexBuffer(SharedPtr<const FVulkanBuffer> _VertexBuffer) const;
		void BindIndexBuffer(SharedPtr<const FVulkanBuffer> _IndexBuffer) const;
		void BindRenderPipeline(SharedPtr<const FVulkanRenderPipeline> _RenderPipeline);
		void SetScissor() const;
		void SetViewport() const;
		//void SetDepthBias() const;
		void ClearColorImage(SharedPtr<FVulkanImage> _Image, FClearColor _ClearColor = { 0,0,0,1 }) const;
		//void ClearAttachment() const;
		void inline
		Draw(UInt32 _VertexCount, UInt32 _InstanceCount = 1, UInt32 _FirstVertex = 0, UInt32 _FirstInstance = 0) const;
		void inline
		DrawIndexed(UInt32 _IndexCount, UInt32 _InstanceCount = 1, UInt32 _FirstIndex = 0, UInt32 _VertexOffset = 0, UInt32 _FirstInstance = 0) const;
		void LeaveRenderPass(SharedPtr<const FVulkanRenderPass> _RenderPass);

		void ConvertImageLayout(VkImage _Image, EVulkanImageLayout _OldLayout, EVulkanImageLayout _NewLayout,
			                    EVulkanImageAspect _Aspect, UInt32 _LevelCount, UInt32 _LayerCount) const;
		void ConvertImageLayout(SharedPtr<FVulkanImage> _Image, EVulkanImageLayout _NewLayout) const;
		void BlitImage(SharedPtr<const FVulkanImage> _SrcImage, SharedPtr<FVulkanImage> _DstImage, EVulkanFilter _Filter) const;

		Bool IsInsideRenderPass() const { return CurrentRenderPass != nullptr; }

	private:
		Segment<VkViewport,1>				CurrentViewports{ VkViewport{.minDepth = 0.0f, .maxDepth = 1.0f} };
		Segment<VkRect2D, 1>				CurrentScissors{ VkRect2D {.offset{.x = 0, .y = 0}, .extent{.width = 0, .height = 0} } };
		SharedPtr<const FVulkanRenderPass>	CurrentRenderPass;

	public:
		FVulkanGraphicsCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner, EVulkanCommandLevel _Level) noexcept
			:FVulkanCommandBuffer{_Owner, EVulkanQueueFamily::Graphics, _Level}{}
		FVulkanGraphicsCommandBuffer() noexcept = delete;
	};

	void FVulkanGraphicsCommandBuffer::
	ClearColorImage(SharedPtr<FVulkanImage> _Image, FClearColor _ClearColor/* = { 0,0,0,1 }*/) const
	{
		auto ResourceRange = _Image->GetResourceRange();
		vkCmdClearColorImage(
			Handle,
			_Image->GetHandle(),
			AutoCast(_Image->GetLayout()),
			&_ClearColor,
			1,
			&ResourceRange
		);
	}

	void FVulkanGraphicsCommandBuffer::
	ConvertImageLayout(VkImage _Image,
		EVulkanImageLayout     _OldLayout,
		EVulkanImageLayout     _NewLayout,
		EVulkanImageAspect     _Aspect,
		UInt32                 _LevelCount,
		UInt32                 _LayerCount) const
	{
		if (_OldLayout == _NewLayout) { return; }

		VkImageMemoryBarrier ImageMemoryBarrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = AutoCast(EVulkanAccess::R_Memory),
			.dstAccessMask = AutoCast(EVulkanAccess::None),
			.oldLayout = AutoCast(_OldLayout),
			.newLayout = AutoCast(_NewLayout),
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = _Image,
			.subresourceRange
			{
				.aspectMask		= AutoCast(_Aspect),
				.baseMipLevel	= 0,
				.levelCount		= _LevelCount,
				.baseArrayLayer = 0,
				.layerCount		= _LayerCount,
			}
		};
		vkCmdPipelineBarrier(
			Handle,
			AutoCast(EVulkanGraphicsPipelineStage::PipelineTop),
			AutoCast(EVulkanGraphicsPipelineStage::PipelineBottom),
			0x0,		// Dependency Flags
			0, nullptr,	// Memory Barrier
			0, nullptr,	// Buffer Memory Barrier
			1,
			&ImageMemoryBarrier);
	}

	void FVulkanGraphicsCommandBuffer::
	ConvertImageLayout(SharedPtr<FVulkanImage> _Image, EVulkanImageLayout _NewLayout) const
	{
		ConvertImageLayout(_Image->Handle, _Image->Layout, _NewLayout, _Image->Aspects, _Image->MipmapLevels, _Image->ArrayLayers);
		_Image->Layout = _NewLayout;
	}

	void FVulkanGraphicsCommandBuffer::
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
	PushConstants(EVulkanShaderStage _ShaderStages, const void* _Data, UInt32 _Size, UInt32 _Offset)
	{
		if (CurrentPipeline != nullptr)
		{
			VE_ASSERT(!CurrentPipeline->HasPushConstant());
			vkCmdPushConstants(
				Handle,
				CurrentPipeline->GetLayout()->GetHandle(),
				AutoCast(_ShaderStages),
				_Offset,
				_Size,
				_Data
			);
		}
		else VE_LOG_FATAL("Call BindPipeline(...) before PushConstants(...)!");
	}

	void FVulkanGraphicsCommandBuffer::
	SetScissor() const
	{
		vkCmdSetScissorWithCount(Handle, CurrentScissors.size(), CurrentScissors.data());
	}

	void FVulkanGraphicsCommandBuffer::
	SetViewport() const
	{
		vkCmdSetViewportWithCount(Handle, CurrentViewports.size(), CurrentViewports.data());
	}

	void FVulkanCommandBuffer::
	BindDescriptorSet(UInt32 _Index, SharedPtr<const FVulkanDescriptorSet> _DescriptorSet)
	{
		auto DescriptorSetHandle = _DescriptorSet->GetHandle();
		if (CurrentPipeline != nullptr)
		{
			vkCmdBindDescriptorSets(Handle,
				AutoCast(CurrentPipeline->GetBindPoint()),
				CurrentPipeline->GetLayout()->GetHandle(),
				_Index,
				1,
				&DescriptorSetHandle,
				0,
				nullptr);
		}
		else VE_LOG_FATAL("Call BindPipeline(...) before BindDescriptorSet(...)!");
	}

	void FVulkanGraphicsCommandBuffer::
	BindVertexBuffer(SharedPtr<const FVulkanBuffer> _VertexBuffer) const
	{
		VE_ASSERT(IsRecording());
		VE_ASSERT(CurrentPipeline != nullptr);
		VE_ASSERT(Bool(_VertexBuffer->GetUsages() & EVulkanBufferUsage::Vertex));

		auto VertexBufferHanedle = _VertexBuffer->GetHandle();
		VkDeviceSize Offset = 0;
		vkCmdBindVertexBuffers(Handle, 0, 1, &VertexBufferHanedle, &Offset);
	}

	void FVulkanGraphicsCommandBuffer::
	BindIndexBuffer(SharedPtr<const FVulkanBuffer> _IndexBuffer) const
	{
		VE_ASSERT(IsRecording());
		VE_ASSERT(CurrentPipeline != nullptr);
		VE_ASSERT(Bool(_IndexBuffer->GetUsages() & EVulkanBufferUsage::Index));

		auto IndexBufferHanedle = _IndexBuffer->GetHandle();
		vkCmdBindIndexBuffer(Handle, IndexBufferHanedle, 0, VK_INDEX_TYPE_UINT32);
	}

	void FVulkanGraphicsCommandBuffer::
	ReachRenderPass(SharedPtr<const FVulkanRenderPass> _RenderPass)
	{ 
		VE_ASSERT(IsRecording());
		CurrentRenderPass = std::move(_RenderPass);

		static VkRenderPassBeginInfo RenderPassBeginInfo{};
		RenderPassBeginInfo = std::move(CurrentRenderPass->GetRenderPassBeginInfo());

		vkCmdBeginRenderPass(
			Handle,
			&RenderPassBeginInfo,
			IsPrimary()? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		const auto& RenderArea = CurrentRenderPass->GetRenderArea();
		CurrentViewports[0].x = RenderArea.offset.x;
		CurrentViewports[0].y = RenderArea.offset.y;
		CurrentViewports[0].width  = RenderArea.extent.width;
		CurrentViewports[0].height = RenderArea.extent.height;
		CurrentScissors[0].extent.width  = RenderArea.extent.width;
		CurrentScissors[0].extent.height = RenderArea.extent.height;
	}

	void FVulkanGraphicsCommandBuffer::
	LeaveRenderPass(SharedPtr<const FVulkanRenderPass> _RenderPass)
	{
		VE_ASSERT(IsInsideRenderPass());
		VE_ASSERT(_RenderPass == CurrentRenderPass);

		vkCmdEndRenderPass(Handle);
		CurrentPipeline.reset();
		CurrentRenderPass.reset();
	}

	void FVulkanCommandBuffer::
	BindPipeline(SharedPtr<const FVulkanPipeline> _Pipeline)
	{
		CurrentPipeline = std::move(_Pipeline);
		vkCmdBindPipeline(Handle, AutoCast(CurrentPipeline->GetBindPoint()), CurrentPipeline->GetHandle());
	}
	
	FVulkanCommandBuffer::
	FVulkanCommandBuffer(SharedPtr<const FVulkanCommandPool> _Owner,
						 EVulkanQueueFamily _QueueFamily,
						 EVulkanCommandLevel _Level) noexcept
		: 
		Owner { _Owner },
		QueueFamily { _QueueFamily },
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
		VE_ASSERT(IsIdle() && "Make sure the Status is setted as Idle at the beginning of each RHIFrame!");
		
		//Implicitly Reset Current CommandBuffer.
		VkCommandBufferBeginInfo BeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (vkBeginCommandBuffer(Handle, &BeginInfo) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to begin recording Vulkan Command FVulkanBuffer!"); }

		Status = EStatus::Recording;
	}

	void FVulkanCommandBuffer::
	StopRecording()
	{
		VE_ASSERT(IsRecording());

		if (vkEndCommandBuffer(Handle) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to stop recording Vulkan Command FVulkanBuffer!"); }

		CurrentPipeline.reset();
		Status = EStatus::ReadyToSubmit;
	}

	void FVulkanCommandBuffer::
	WriteBuffer(SharedPtr<FVulkanBuffer>       _Destination,
		        UInt64                         _DestOffset,
		        SharedPtr<const FVulkanBuffer> _Source,
		        UInt64                         _SrcOffset,
		        UInt64                         _Size)
	{
		VE_ASSERT(IsRecording() && _Destination && _Source && _Size);
		VE_ASSERT(_Destination->GetSize() >= _Source->GetSize());
		VE_ASSERT(Bool(_Source->GetUsages() & EVulkanBufferUsage::TransferSource));
		VE_ASSERT(Bool(_Destination->GetUsages() & EVulkanBufferUsage::TransferDestination));

		VkBufferCopy CopyInfo
		{
			.srcOffset = _SrcOffset,
			.dstOffset = 0,
			.size = _Source->GetSize(),
		};
		vkCmdCopyBuffer(Handle, _Source->GetHandle(), _Destination->GetHandle(), 1, &CopyInfo);
	}

	void FVulkanCommandBuffer::
	WriteBuffer(SharedPtr<FVulkanBuffer> _Destination, SharedPtr<const FVulkanBuffer> _Source, UInt64 _Size/* = 0*/)
	{
		WriteBuffer(_Destination, 0, _Source, 0, _Size? _Size : _Source->GetSize());
	}

	void FVulkanGraphicsCommandBuffer::
	BindRenderPipeline(SharedPtr<const FVulkanRenderPipeline> _RenderPipeline)
	{ 
		VE_ASSERT(IsInsideRenderPass() && CurrentRenderPass->HasSubpass(_RenderPipeline));

		FVulkanCommandBuffer::BindPipeline(_RenderPipeline);
		//Assert Enable Dynamic Scissors & Viewports
		SetScissor();
		SetViewport();
	}

	void FVulkanGraphicsCommandBuffer::
	Submit(const VkSemaphore*                  _pWaitSemaphores,   UInt32 _WaitSemaphoreCount,
		   const EVulkanGraphicsPipelineStage* _pWaitStages,    // WaitStageCount == _WaitSemaphoreCount
		   const VkSemaphore*                  _pSignalSemaphores, UInt32 _SignalSemaphoreCount,
		   const FVulkanFence*                 _SignalFence/* = nullptr*/) const
	{
		VE_ASSERT(IsReadyToSubmit());

		VkSubmitInfo SubmitInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext					= nullptr,
			.waitSemaphoreCount		= _WaitSemaphoreCount,
			.pWaitSemaphores		= _pWaitSemaphores,
			.pWaitDstStageMask		= reinterpret_cast<const VkPipelineStageFlags*>(_pWaitStages),
			.commandBufferCount		= 1,
			.pCommandBuffers		= &Handle,
			.signalSemaphoreCount	= _SignalSemaphoreCount,
			.pSignalSemaphores		= _pSignalSemaphores,
		};

		auto& GraphicsQueueFamily = GVulkan->Device->GetQueueFamily(EVulkanQueueFamily::Graphics);
		if (vkQueueSubmit(
			GraphicsQueueFamily.Queues[0],
			1,
			&SubmitInfo,
			_SignalFence? _SignalFence->GetHandle() : VK_NULL_HANDLE) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to submit the Vulkan Graphics CommandBuffer (handle:{}, queue:{})", (Address)(Handle), 0); }

		Status = EStatus::Submitted;
	}

	void FVulkanGraphicsCommandBuffer::
	WriteImage(SharedPtr<FVulkanImage> _Image, SharedPtr<const FVulkanBuffer> _StagingBuffer)
	{
		if (_Image->GetLayout() != EVulkanImageLayout::TransferDestination)
		{ VE_LOG_FATAL("Failed to write the image! - Not transferable!"); }

		VkBufferImageCopy WriteInfo
		{
			.bufferOffset		= 0,
			.bufferRowLength	= 0,
			.bufferImageHeight	= 0,
			.imageSubresource
			{
				.aspectMask		= AutoCast(_Image->GetAspects()),
				.mipLevel		= 0,
				.baseArrayLayer = 0,
				.layerCount		= 1,
			},
			.imageOffset		= {0, 0, 0},
			.imageExtent		= _Image->GetExtent(),
		};
		vkCmdCopyBufferToImage(
			Handle,
			_StagingBuffer->GetHandle(),
			_Image->GetHandle(),
			AutoCast(_Image->GetLayout()),
			1,
			&WriteInfo);
	}

	void FVulkanGraphicsCommandBuffer::
	Draw(UInt32 _VertexCount,
		 UInt32 _InstanceCount/* = 1*/,
		 UInt32 _FirstVertex  /* = 0*/,
		 UInt32 _FirstInstance/* = 0*/) const
	{ 
		VE_ASSERT(IsRecording());
		VE_ASSERT(CurrentPipeline != nullptr && "Call BindPipeline()");
		vkCmdDraw(Handle, _VertexCount, _InstanceCount, _FirstVertex, _FirstInstance);
	}

	void FVulkanGraphicsCommandBuffer::
	DrawIndexed(UInt32 _IndexCount,
		        UInt32 _InstanceCount/* = 1*/,
		        UInt32 _FirstIndex   /* = 0*/,
		        UInt32 _VertexOffset /* = 0*/,
		        UInt32 _FirstInstance/* = 0*/) const
	{ 
		VE_ASSERT(IsRecording());
		VE_ASSERT(CurrentPipeline != nullptr && "Call BindPipeline()");
		vkCmdDrawIndexed(Handle, _IndexCount, _InstanceCount, _FirstIndex, _VertexOffset, _FirstInstance);
	}
	
} // namespace VE