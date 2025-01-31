module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:CommandBuffer;

import :Enums;
import :Device;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{ 
	class FVulkanCommandPool;

	class FVulkanCommandBuffer
	{
		friend class FVulkanCommandPool;
	public:
		void StartRecording();
		void StopRecording();
		void Reset()			  { VE_ASSERT(!IsRecording() && IsResettable()); vkResetCommandBuffer(Handle, 0x0); }

		Bool IsRecording()	const { return bIsRecording;    }
		Bool IsResettable() const { return bIsResettable;   }
		Bool IsPrimary()	const { return bIsPrimary;		}
		auto GetLevel()		const -> ECommandLevel { return IsPrimary()? ECommandLevel::Primary : ECommandLevel::Secondary; }
		auto GetHandle()		  -> VkCommandBuffer { return Handle; }
		operator VkCommandBuffer() { return Handle; }

	private:
		VkCommandBuffer				  Handle{ VK_NULL_HANDLE };
		SharedPtr<FVulkanCommandPool> HostCommandPool;
		Array<VkViewport>			  CurrentViewports;
		Array<VkRect2D>				  CurrentScissors;

		Byte bIsRecording	: 1;
		Byte bIsResettable  : 1;
		Byte bIsPrimary		: 1;

	public:
		FVulkanCommandBuffer(ECommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};

	
	FVulkanCommandBuffer::
	FVulkanCommandBuffer(ECommandLevel _Level) noexcept
		: bIsPrimary { ECommandLevel::Primary == _Level? True : False }
	{
		
	}

	FVulkanCommandBuffer::
	~FVulkanCommandBuffer() noexcept
	{
		VE_ASSERT(Handle == VK_NULL_HANDLE); // Clean Commandbuffers in the CommandPool!
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

		bIsRecording = True;
	}

	void FVulkanCommandBuffer::
	StopRecording()
	{
		VE_ASSERT(IsRecording());

		if (VK_SUCCESS != vkEndCommandBuffer(Handle))
		{ throw SRuntimeError("Failed to stop recording Vulkan Command FBuffer!"); }

		bIsRecording = False;
	}
	
} } // namespace VE::Runtime