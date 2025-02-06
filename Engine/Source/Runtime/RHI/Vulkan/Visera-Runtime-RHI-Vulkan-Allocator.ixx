module;
#define VMA_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Allocator;
import :Common;
import :Instance;
import :Device;
import :GPU;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanBuffer;
	class FVulkanImage;

	class FVulkanAllocator
	{
		friend class FVulkan;
	public:
		auto CreateBuffer(VkDeviceSize _Size,
						  EBufferUsage _Usages,
						  ESharingMode _SharingMode = ESharingMode::Exclusive,
						  EMemoryUsage _Location    = EMemoryUsage::Auto) const -> SharedPtr<FVulkanBuffer>;
		auto CreateImage(EImageType		_Type,
						FVulkanExtent	_Extent,
						EImageAspect	_Aspects,
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
		
	class FVulkanImageView
	{
		friend class FVulkanImage;
	public:
		auto GetImage()  const  -> WeakPtr<const FVulkanImage>	{ return Image; }
		auto GetHandle() const  -> const VkImageView			{ return Handle;}

		Bool IsExpired() const									{ return Image.expired(); }

		FVulkanImageView() = delete;
		FVulkanImageView(SharedPtr<const FVulkanImage> _SourceImage) : Image{ _SourceImage } {}
		~FVulkanImageView() { vkDestroyImageView(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }

	private:
		VkImageView					Handle{ VK_NULL_HANDLE };
		WeakPtr<const FVulkanImage>	Image;
		EImageViewType				TypeView;
		EImageAspect				AspectView;
		EFormat						FormatView;
		Pair<UInt8, UInt8>			MipmapLevelRange{ 0, 0 };
		Pair<UInt8, UInt8>			ArrayLayerRange	{ 0, 0 };
	};

	class FVulkanImage : public std::enable_shared_from_this<FVulkanImage>
	{
		friend class FVulkanAllocator;
	public:
		auto CreateImageView(
			EImageViewType		_Type,
			EFormat				_Format = EFormat::None,
			EImageAspect		_Aspect = EImageAspect::Undefined,
			Pair<UInt8, UInt8> _MipmapLevelRange = {0,0},
			Pair<UInt8, UInt8> _ArrayLayerRange  = {0,0},
			const FVulkanComponentMapping& _ComponentMapping = {}) const->SharedPtr<FVulkanImageView>;

		auto GetExtent()    const -> const FVulkanExtent&	{ return Extent; }
		auto GetSize()		const -> VkDeviceSize			{ return Allocation->GetSize(); }
		auto GetFormat()	const -> EFormat				{ return Format; }
		auto GetAspects()   const -> EImageAspect			{ return Aspects; }
		auto GetUsages()	const -> EImageUsage			{ return Usages; }
		auto GetTiling()	const -> EImageTiling			{ return Tiling; }
		auto GetMipmapLevels()	const -> UInt8				{ return MipmapLevels; }
		auto GetArrayLayers()	const -> UInt8				{ return ArrayLayers; }
		auto GetSampleRate()	const -> ESampleRate		{ return SampleRate;}
		auto GetDetails()	const -> VmaAllocationInfo  { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }

		auto GetHandle()	const -> const VkImage { return Handle; }

	protected:
		VkImage			Handle;
		EImageType		Type;
		EFormat			Format;
		FVulkanExtent	Extent;
		EImageAspect    Aspects;
		EImageUsage		Usages;
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
				FVulkanExtent	_Extent,
				EImageAspect	_Aspects,
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

		return NewImage;
	}

	FVulkanImage::
	~FVulkanImage()
	{
		vmaDestroyImage(GVulkan->Allocator->GetHandle(), Handle, Allocation);
		Handle = VK_NULL_HANDLE;
	}
	
	SharedPtr<FVulkanImageView> FVulkanImage::
	CreateImageView(EImageViewType	   _Type,
					EFormat			   _Format/* = EFormat::None*/,
					EImageAspect	   _Aspect/* = EImageAspect::Undefined*/,
					Pair<UInt8, UInt8> _MipmapLevelRange/* = { 0,0 }*/,
					Pair<UInt8, UInt8> _ArrayLayerRange /* = { 0,0 }*/,
					const FVulkanComponentMapping& _ComponentMapping/* = {}*/) const
	{
		//[TODO]: Check more ImageView validity.
		VE_ASSERT(IsOrderedPair(_MipmapLevelRange) && IsOrderedPair(_ArrayLayerRange));

		auto NewImageView = CreateSharedPtr<FVulkanImageView>(shared_from_this());
		NewImageView->TypeView   = _Type;
		NewImageView->FormatView = (_Format == EFormat::None)? this->Format : _Format;
		NewImageView->AspectView = (_Aspect == EImageAspect::Undefined)? this->Aspects : _Aspect;
		NewImageView->MipmapLevelRange = std::move(_MipmapLevelRange);
		NewImageView->ArrayLayerRange  = std::move(_ArrayLayerRange);

		VkImageViewCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = this->Handle,
			.viewType = AutoCast(NewImageView->TypeView),
			.format   = AutoCast(NewImageView->FormatView),
			.components
			{
				 .r = AutoCast(_ComponentMapping.RMapping),
				 .g = AutoCast(_ComponentMapping.GMapping),
				 .b = AutoCast(_ComponentMapping.BMapping),
				 .a = AutoCast(_ComponentMapping.AMapping),
			},
			.subresourceRange
			{
				.aspectMask		= AutoCast(NewImageView->AspectView),
				.baseMipLevel	= NewImageView->MipmapLevelRange.first,
				.levelCount		= UInt32(NewImageView->MipmapLevelRange.second - NewImageView->MipmapLevelRange.first) + 1,
				.baseArrayLayer = NewImageView->ArrayLayerRange.first,
				.layerCount     = UInt32(NewImageView->ArrayLayerRange.second - NewImageView->ArrayLayerRange.first) + 1,
			}
		};
		if (vkCreateImageView(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&(NewImageView->Handle)) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Image View!"); }

		return NewImageView;
	}

	SharedPtr<FVulkanBuffer> FVulkanAllocator::
	CreateBuffer(VkDeviceSize _Size,
				 EBufferUsage _Usages,
				 ESharingMode _SharingMode/* = ESharingMode::Exclusive*/,
				 EMemoryUsage _Location/* = EMemoryUsage::Auto*/) const
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
			.sharingMode = AutoCast(_SharingMode),
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};

		VmaAllocationCreateInfo AllocationCreateInfo
		{
			.flags = 0x0,
			.usage = AutoCast(_Location)
		};

		if(vmaCreateBuffer(
			Handle,
			&CreateInfo,
			&AllocationCreateInfo,
			&NewBuffer->Handle,
			&NewBuffer->Allocation,
			nullptr) != VK_SUCCESS)
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