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
	class FVulkanBuffer;
	class FVulkanImage;

	class FVulkanAllocator
	{
		friend class FVulkan;
	public:
		auto CreateBuffer(VkDeviceSize _Size, EBufferUsage _Usages, EMemoryUsage Location = EMemoryUsage::Auto) const -> SharedPtr<FVulkanBuffer>;

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

	class FVulkanBuffer
	{
		friend class FVulkanAllocator;
	public:
		auto GetSize() const -> VkDeviceSize { return Allocation->GetSize(); }
		auto GetDetails() const -> VmaAllocationInfo { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }

		auto GetHandle() -> VkBuffer { return Handle; }

	private:
		VkBuffer		Handle;
		VmaAllocation	Allocation;

	public:	
		~FVulkanBuffer() noexcept;
	};

	class FVulkanImage
	{
		friend class FVulkanAllocator;
	public:
		auto GetSize() const -> VkDeviceSize { return Allocation->GetSize(); }
		auto GetDetails() const -> VmaAllocationInfo { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }

		auto GetHandle() -> VkImage { return Handle; }

	private:
		VkImage			Handle;
		VmaAllocation	Allocation;

	public:	
		~FVulkanImage() noexcept;
	};


	SharedPtr<FVulkanBuffer>
	FVulkanAllocator::
	CreateBuffer(VkDeviceSize _Size, EBufferUsage _Usages, EMemoryUsage Location/* = EMemoryUsage::Auto*/) const
	{
		VE_ASSERT(_Size > 0);

		auto NewBuffer = CreateSharedPtr<FVulkanBuffer>();
		VkBufferCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.size  = _Size,
			.usage = AutoCast(_Usages),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};

		VmaAllocationCreateInfo AllocationCreateInfo
		{
			.flags = 0x0,
			.usage = AutoCast(Location)
		};

		if(VK_SUCCESS != vmaCreateBuffer(
			Handle,
			&CreateInfo,
			&AllocationCreateInfo,
			&NewBuffer->Handle,
			&NewBuffer->Allocation,
			nullptr))
		{ throw SRuntimeError("Failed to create VMA FVulkanBuffer!"); }

		return NewBuffer;
	}

	FVulkanBuffer::
	~FVulkanBuffer() noexcept
	{
		vmaDestroyBuffer(GVulkan->Allocator->GetHandle(), Handle, Allocation);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime