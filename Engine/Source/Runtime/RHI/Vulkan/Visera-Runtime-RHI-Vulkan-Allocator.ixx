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

	union FImageExtent
	{
		VkExtent2D Extent2D;
		VkExtent3D Extent3D{ 0 };
	};

	class FVulkanAllocator
	{
		friend class FVulkan;
	public:
		auto CreateBuffer(VkDeviceSize _Size, EBufferUsage _Usages, EMemoryUsage _Location = EMemoryUsage::Auto) const -> SharedPtr<FVulkanBuffer>;
		auto CreateImage(EImageType		_Type,
						FImageExtent	_Extent,
						EImageAspect	_Aspect,
						EImageUsage		_Usages,
						EImageTiling	_Tiling = EImageTiling::Optimal,
						ESampleRate     _SampleRate = ESampleRate::X1,
						EMemoryUsage	_Location = EMemoryUsage::Auto) const -> SharedPtr<FVulkanImage>;

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
			if (vmaCreateAllocator(&CreateInfo, &Handle) != VK_SUCCESS)
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
		
	class FImageView
	{
		friend class FVulkanImage;
	public:
		auto GetImage()  const  -> WeakPtr<FVulkanImage> { return Image; }
		Bool IsExpired() const							 { return Image.expired(); }

	private:
		VkImageView				Handle{ VK_NULL_HANDLE };
		EImageViewType			Type;
		WeakPtr<FVulkanImage>	Image;
		EFormat					Format;
		Pair<UInt8, UInt8>		MipmapLevelRange{ 0, 0 };
		Pair<UInt8, UInt8>		ArrayLayerRange	{ 0, 0 };
	};

	class FVulkanImage : public std::enable_shared_from_this<FVulkanImage>
	{
		friend class FVulkanAllocator;
	public:
		auto CreateImageView(EFormat _Format, Pair<UInt8, UInt8> _MipmapLevelRange = {0,0}, Pair<UInt8, UInt8> _ArrayLayerRange = {0,0}) const->SharedPtr<FImageView>;

		auto GetExtent()    const -> const FImageExtent&{ return Extent; }
		auto GetSize()		const -> VkDeviceSize   { return Allocation->GetSize(); }
		auto GetDetails()	const -> VmaAllocationInfo { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }

		auto GetHandle()		  -> VkImage { return Handle; }

	protected:
		VkImage			Handle;
		EImageType		Type;
		EFormat			Format;
		FImageExtent	Extent;
		EImageAspect    Aspect;
		EImageUsage		Usage;
		EImageTiling	Tiling;
		UInt8			MipmapLevels = 0;
		UInt8			ArrayLayers	 = 0;
		ESampleRate     SampleRate	 = ESampleRate::X1;

		VmaAllocation	Allocation;
	public:	
		FVulkanImage()  = default;
		~FVulkanImage();
	};

	SharedPtr<FVulkanImage> FVulkanAllocator::
	CreateImage(EImageType		_Type,
				FImageExtent	_Extent,
				EImageAspect	_Aspect,
				EImageUsage		_Usages,
				EImageTiling	_Tiling		/*= EImageTiling::Optimal*/,
				ESampleRate     _SampleRate /*= ESampleRate::X1*/,
				EMemoryUsage	_Location	/*= EMemoryUsage::Auto*/) const
	{
		auto NewImage = CreateSharedPtr<FVulkanImage>();
		VE_WIP;
		/*VkImageCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.imageType,
		}*/
	}
	
	SharedPtr<FImageView> FVulkanImage::
	CreateImageView(EFormat _Format,
					Pair<UInt8, UInt8> _MipmapLevelRange/* = { 0,0 }*/,
					Pair<UInt8, UInt8> _ArrayLayerRange /* = { 0,0 }*/) const
	{
		auto ImageView = CreateSharedPtr<FImageView>();
		ImageView->Format = _Format;
		ImageView->MipmapLevelRange = std::move(_MipmapLevelRange);
		ImageView->ArrayLayerRange  = std::move(_ArrayLayerRange);
		/*
		VkImageViewCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = Handle,
			.viewType = 
		}
		*/
	}

	FVulkanImage::
	~FVulkanImage()
	{

	}

	SharedPtr<FVulkanBuffer> FVulkanAllocator::
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