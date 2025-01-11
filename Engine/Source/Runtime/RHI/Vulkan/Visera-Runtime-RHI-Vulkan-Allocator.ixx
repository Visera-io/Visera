module;
#define VMA_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Allocator;

import Visera.Core.Signal;

import :Enums;
import :Instance;
import :Device;
import :GPU;

export namespace VE { namespace Runtime
{


class FVulkanAllocator
{
	friend class FVulkan;
public:
	class Buffer
	{
		friend class FVulkanAllocator;
	public:
		struct CreateInfo
		{
			EBufferUsage Usages   = EBufferUsage::None;
			EMemoryUsage Location = EMemoryUsage::Auto;
			VkDeviceSize Size = 0;
		};
		auto GetSize() const -> VkDeviceSize { return Allocation->GetSize(); }
		auto GetDetails() const -> VmaAllocationInfo { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }

		auto GetHandle() const -> VkBuffer { return Handle; }
		operator VkBuffer() const { return Handle; }

	private:
		VkBuffer		Handle;
		VmaAllocation	Allocation;

	public:	
		~Buffer() noexcept;
	};

public:
	auto CreateBuffer(const Buffer::CreateInfo& _CreateInfo) const -> SharedPtr<Buffer>;

	auto GetHandle() const -> VmaAllocator { return Handle; }
	operator VmaAllocator() const { return Handle; }
	
private:
	void Create()
	{
		VmaVulkanFunctions VulkanFunctions
		{
			.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
			.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
			.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements
		};
			
		VmaAllocatorCreateInfo CreateInfo
		{
			.physicalDevice = GVulkan->GPU->GetHandle(),
			.device = GVulkan->Device->GetHandle(),
			.pVulkanFunctions = &VulkanFunctions,
			.instance = GVulkan->Instance->GetHandle(),
			.vulkanApiVersion = GVulkan->Instance->GetVulkanAPIVersion()
		};
		if (VK_SUCCESS != vmaCreateAllocator(&CreateInfo, &Handle))
		{ throw SRuntimeError("Failed to create VMA Allocator!"); }
	}

	void Destory()
	{
		vmaDestroyAllocator(Handle);
		Handle = VMA_NULL;
	}

private:
	VmaAllocator	Handle{ VMA_NULL };
};

SharedPtr<FVulkanAllocator::Buffer>
FVulkanAllocator::
CreateBuffer(const Buffer::CreateInfo& _CreateInfo) const
{
	VE_ASSERT(_CreateInfo.Size > 0);

	auto NewBuffer = CreateSharedPtr<Buffer>();
	VkBufferCreateInfo CreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0x0,
		.size = _CreateInfo.Size,
		.usage = AutoCast(_CreateInfo.Usages),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
	};

	VmaAllocationCreateInfo AllocationCreateInfo
	{
		.flags = 0x0,
		.usage = VmaMemoryUsage(_CreateInfo.Location)
	};

	if(VK_SUCCESS != vmaCreateBuffer(
		Handle,
		&CreateInfo,
		&AllocationCreateInfo,
		&NewBuffer->Handle,
		&NewBuffer->Allocation,
		nullptr))
	{ throw SRuntimeError("Failed to create VMA Buffer!"); }

	return NewBuffer;
}

FVulkanAllocator::Buffer::
~Buffer() noexcept
{
	vmaDestroyBuffer(GVulkan->Allocator->GetHandle(), Handle, Allocation);
	Handle = VK_NULL_HANDLE;
}

} } // namespace VE::Runtime