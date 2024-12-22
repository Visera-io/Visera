module;
#include <Visera>

#include <volk.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Allocator;

import :Context;
import :Common;
import :Instance;
import :Device;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanAllocator
	{
		friend class Vulkan;
	public:
		class Buffer
		{
			friend class VulkanAllocator;
		public:
			struct CreateInfo
			{
				VulkanBufferUsages::Option Usages   = VulkanBufferUsages::Option::None;
				VulkanMemoryUsages::Option Location = VulkanMemoryUsages::Option::Auto;
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
		auto CreateBuffer(const Buffer::CreateInfo& _CreateInfo) -> SharedPtr<Buffer>;

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
			VK_CHECK(vmaCreateAllocator(&CreateInfo, &Handle));
		}

		void Destory()
		{
			vmaDestroyAllocator(Handle);
			Handle = VMA_NULL;
		}

	private:
		VmaAllocator	Handle{ VMA_NULL };
	};

	SharedPtr<VulkanAllocator::Buffer>
	VulkanAllocator::
	CreateBuffer(const Buffer::CreateInfo& _CreateInfo)
	{
		auto NewBuffer = CreateSharedPtr<Buffer>();
		VkBufferCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.size = _CreateInfo.Size,
			.usage = _CreateInfo.Usages,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};

		VmaAllocationCreateInfo AllocationCreateInfo
		{
			.flags = 0x0,
			.usage = VmaMemoryUsage(_CreateInfo.Location)
		};

		VK_CHECK(vmaCreateBuffer(
			Handle,
			&CreateInfo,
			&AllocationCreateInfo,
			&NewBuffer->Handle,
			&NewBuffer->Allocation,
			nullptr));

		return NewBuffer;
	}

	VulkanAllocator::Buffer::
	~Buffer() noexcept
	{
		vmaDestroyBuffer(GVulkan->Allocator->GetHandle(), Handle, Allocation);
	}

} } // namespace VE::Runtime