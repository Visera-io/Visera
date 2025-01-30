module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:DescriptorSetLayout;

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

		Bool IsRecording()	const { return Flags & RecordingMask;    }
		Bool IsResettable() const { return Flags & ResettableMask;   }
		Bool IsPrimary()	const { return Flags & PrimaryLevelMask; }
		auto GetLevel()		const -> ECommandLevel { return IsPrimary()? ECommandLevel::Primary : ECommandLevel::Secondary; }
		auto GetHandle()    const -> VkCommandBuffer { return Handle; }
		operator VkCommandBuffer() const { return Handle; }

	private:
		VkCommandBuffer				  Handle{ VK_NULL_HANDLE };
		SharedPtr<FVulkanCommandPool> HostCommandPool;
		Byte bHasPipeline : 1;
		Byte bHasPipeline : 1;
		Byte bHasPipeline : 1;
		Byte bHasPipeline : 1;
		Byte bHasPipeline : 1;

	public:
		FVulkanCommandBuffer(ECommandLevel _Level) noexcept;
		FVulkanCommandBuffer() noexcept = delete;
		~FVulkanCommandBuffer() noexcept;
	};

	
	FVulkanCommandBuffer::
	FVulkanCommandBuffer(ECommandLevel _Level) noexcept
		: Flags { ECommandLevel::Primary == _Level? PrimaryLevelMask : 0x0 }
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

		Flags ^= RecordingMask;
	}

	void FVulkanCommandBuffer::
	StopRecording()
	{
		VE_ASSERT(IsRecording());

		if (VK_SUCCESS != vkEndCommandBuffer(Handle))
		{ throw SRuntimeError("Failed to stop recording Vulkan Command FBuffer!"); }

		Flags ^= RecordingMask;
	}
	
} } // namespace VE::Runtime