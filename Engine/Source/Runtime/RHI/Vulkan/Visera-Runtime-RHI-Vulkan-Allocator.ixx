module;
#define VMA_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Allocator;
import :Common;
import :Instance;
import :Device;
import :GPU;

import Visera.Core.Signal;
import Visera.Template.Pattern;

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
						FVulkanExtent3D	_Extent,
						EFormat			_Format,
						EImageAspect	_Aspects,
						EImageUsage		_Usages,
						EImageTiling	_Tiling		  = EImageTiling::Optimal,
						ESampleRate     _SampleRate	  = ESampleRate::X1,
						UInt8			_MipmapLevels = 1,
						UInt8			_ArrayLayers  = 1,
						ESharingMode	_SharingMode  = ESharingMode::Exclusive,
						EMemoryUsage	_Location	  = EMemoryUsage::Auto) const -> SharedPtr<FVulkanImage>;

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

	class FVulkanBuffer : public Template::FPrototype<SharedPtr<FVulkanBuffer>>
	{
		VE_NOT_COPYABLE(FVulkanBuffer);
		friend class FVulkanAllocator;
	public:
		auto GetSize()		const	-> VkDeviceSize { return Allocation->GetSize(); }
		auto GetDetails()	const	-> VmaAllocationInfo { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }
		auto GetHandle()			-> VkBuffer { return Handle; }
		Bool IsReleased()	const { return Handle == VK_NULL_HANDLE && Allocation == VK_NULL_HANDLE; }

		VE_API Free(VkBuffer* _pHandle, VmaAllocation* _pAllocation) { VE_ASSERT(_pHandle && _pAllocation); vmaDestroyBuffer(GVulkan->Allocator->GetHandle(), *_pHandle, *_pAllocation); *_pHandle = VK_NULL_HANDLE;  *_pAllocation = VK_NULL_HANDLE; }
		auto   Release() -> ResultPackage<VkBuffer, VmaAllocation> { VkBuffer RawHandle = Handle; VmaAllocation RawAlloc = Allocation; Handle = VK_NULL_HANDLE; Allocation = VK_NULL_HANDLE; return { RawHandle, RawAlloc }; }
		virtual auto Clone() const -> SharedPtr<FVulkanBuffer> override;
	private:
		VkBuffer		Handle{ VK_NULL_HANDLE };
		VmaAllocation	Allocation;
		EBufferUsage	Usages;
		ESharingMode	SharingMode;
		EMemoryUsage	Location;

	public:	
		FVulkanBuffer() = default;
		FVulkanBuffer(FVulkanBuffer&& _Another)					= default;
		FVulkanBuffer& operator=(FVulkanBuffer&& _Another)		= default;
		~FVulkanBuffer() noexcept;
	};
		
	class FVulkanImageView
	{
		VE_NOT_COPYABLE(FVulkanImageView);
		friend class FVulkanImage;
	public:
		
		auto GetImage()	  const -> WeakPtr<const FVulkanImage>	{ return Image; }
		auto GetHandle()		-> VkImageView			{ return Handle;}

		Bool IsReleased() const { return Handle == VK_NULL_HANDLE; }
		Bool IsExpired()  const	{ return Image.expired(); }

		VE_API Free(VkImageView* _pHandle) { VE_ASSERT(_pHandle); if (*_pHandle) { vkDestroyImageView(GVulkan->Device->GetHandle(), *_pHandle, GVulkan->AllocationCallbacks); *_pHandle = VK_NULL_HANDLE; } }
		auto   Release() -> VkImageView { VkImageView RawHandle = Handle; Handle = VK_NULL_HANDLE; return RawHandle; }
	private:
		VkImageView					Handle{ VK_NULL_HANDLE };
		WeakPtr<const FVulkanImage>	Image;
		EImageViewType				TypeView;
		EImageAspect				AspectView;
		EFormat						FormatView;
		Pair<UInt8, UInt8>			MipmapLevelRange{ 0, 0 };
		Pair<UInt8, UInt8>			ArrayLayerRange	{ 0, 0 };

	public:
		FVulkanImageView() = delete;
		FVulkanImageView(SharedPtr<const FVulkanImage> _SourceImage) : Image{ _SourceImage } {}
		FVulkanImageView(FVulkanImageView&& _Another)					= default;
		FVulkanImageView& operator=(FVulkanImageView&& _Another)		= default;
		~FVulkanImageView() { if (!IsReleased()) { Free(&Handle); } }
	};

	class FVulkanImage : 
		public std::enable_shared_from_this<FVulkanImage>,
		public Template::FPrototype<SharedPtr<FVulkanImage>>
	{
		VE_NOT_COPYABLE(FVulkanImage);
		friend class FVulkanAllocator;
	public:
		auto CreateImageView(
			EImageViewType		_Type   = EImageViewType::Auto,
			EFormat				_Format = EFormat::None,
			EImageAspect		_Aspect = EImageAspect::Undefined,
			Pair<UInt8, UInt8> _MipmapLevelRange = {0,0},
			Pair<UInt8, UInt8> _ArrayLayerRange  = {0,0},
			const FVulkanComponentMapping& _ComponentMapping = {}) const -> SharedPtr<FVulkanImageView>;

		auto GetExtent()		const -> const FVulkanExtent3D&	{ return Extent; }
		auto GetSize()			const -> VkDeviceSize			{ return Allocation->GetSize(); }
		auto GetType()			const -> EImageType				{ return Type; }
		auto GetFormat()		const -> EFormat				{ return Format; }
		auto GetAspects()		const -> EImageAspect			{ return Aspects; }
		auto GetUsages()		const -> EImageUsage			{ return Usages; }
		auto GetTiling()		const -> EImageTiling			{ return Tiling; }
		auto GetMipmapLevels()	const -> UInt8				{ return MipmapLevels; }
		auto GetArrayLayers()	const -> UInt8				{ return ArrayLayers; }
		auto GetSampleRate()	const -> ESampleRate		{ return SampleRate;}
		auto GetDetails()		const -> VmaAllocationInfo  { VmaAllocationInfo Info; vmaGetAllocationInfo(GVulkan->Allocator->GetHandle(), Allocation, &Info); return Info; }
		auto GetHandle()			  -> VkImage { return Handle; }

		Bool IsReleased() const { return Handle == VK_NULL_HANDLE && Allocation == VK_NULL_HANDLE; }
		
		VE_API Free(VkImage* _pHandle, VmaAllocation* _pAllocation) { VE_ASSERT(_pHandle && _pAllocation); vmaDestroyImage(GVulkan->Allocator->GetHandle(), *_pHandle, *_pAllocation); *_pHandle = VK_NULL_HANDLE;  *_pAllocation = VK_NULL_HANDLE; }
		auto   Release() -> ResultPackage<VkImage, VmaAllocation> { VkImage RawHandle = Handle; VmaAllocation RawAlloc = Allocation; Handle = VK_NULL_HANDLE; Allocation = VK_NULL_HANDLE; return { RawHandle, RawAlloc }; }
		virtual auto Clone() const -> SharedPtr<FVulkanImage> override;
	protected:
		VkImage			Handle;
		EImageType		Type;
		EFormat			Format;
		FVulkanExtent3D	Extent;
		EImageAspect    Aspects;
		EImageUsage		Usages;
		EImageTiling	Tiling;
		UInt8			MipmapLevels = 0;
		UInt8			ArrayLayers	 = 0;
		ESampleRate     SampleRate	 = ESampleRate::X1;
		ESharingMode	SharingMode;
		EMemoryUsage	Location;

		VmaAllocation	Allocation;
	public:	
		FVulkanImage() = default;
		FVulkanImage(FVulkanImage&& _Another)					= default;
		FVulkanImage& operator=(FVulkanImage&& _Another)		= default;
		~FVulkanImage();
	};

	SharedPtr<FVulkanImage> FVulkanAllocator::
	CreateImage(EImageType		_Type,
				FVulkanExtent3D	_Extent,
				EFormat			_Format,
				EImageAspect	_Aspects,
				EImageUsage		_Usages,
				EImageTiling	_Tiling		/*= EImageTiling::Optimal*/,
				ESampleRate     _SampleRate /*= ESampleRate::X1*/,
				UInt8			_MipmapLevels/* = 1*/,
				UInt8			_ArrayLayers/* = 1*/,
				ESharingMode	_SharingMode/* = ESharingMode::Exclusive*/,
				EMemoryUsage	_Location	/*= EMemoryUsage::Auto*/) const
	{
		auto NewImage = CreateSharedPtr<FVulkanImage>();
		NewImage->Type			= _Type;
		NewImage->Extent		= std::move(_Extent);
		NewImage->Format		= _Format;
		NewImage->Aspects		= _Aspects;
		NewImage->Usages		= _Usages;
		NewImage->Tiling		= _Tiling;
		NewImage->SampleRate	= _SampleRate;
		NewImage->MipmapLevels	= _MipmapLevels;
		NewImage->ArrayLayers	= _ArrayLayers;
		NewImage->SharingMode   = _SharingMode;
		NewImage->Location		= _Location;
		
		VkImageCreateInfo CreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext					= nullptr,
			.flags					= 0x0,
			.imageType				= AutoCast(NewImage->Type),
			.format					= AutoCast(NewImage->Format),
			.extent					= NewImage->Extent,
			.mipLevels				= NewImage->MipmapLevels,
			.arrayLayers			= NewImage->ArrayLayers,
			.samples				= AutoCast(NewImage->SampleRate),
			.tiling					= AutoCast(NewImage->Tiling),
			.usage					= AutoCast(NewImage->Usages),
			.sharingMode			= AutoCast(_SharingMode),
			//.queueFamilyIndexCount	= 1,
			//.pQueueFamilyIndices	= &(GVulkan->Device->GetQueueFamily(EQueueFamily::Graphics).Index),
			.initialLayout			= AutoCast(EImageLayout::Undefined),
		};

		VmaAllocationCreateInfo AllocationCreateInfo
		{
			.flags = 0x0,
			.usage = AutoCast(_Location),
		};

		if (vmaCreateImage(
			Handle,
			&CreateInfo,
			&AllocationCreateInfo,
			&NewImage->Handle,
			&NewImage->Allocation,
			nullptr) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Image!"); }

		return NewImage;
	}

	FVulkanImage::
	~FVulkanImage()
	{
		if (!IsReleased())
		{ Free(&Handle, &Allocation); }
	}
	
	SharedPtr<FVulkanImageView> FVulkanImage::
	CreateImageView(EImageViewType	   _Type  /*= EImageViewType::Auto*/,
					EFormat			   _Format/* = EFormat::None*/,
					EImageAspect	   _Aspect/* = EImageAspect::Undefined*/,
					Pair<UInt8, UInt8> _MipmapLevelRange/* = { 0,0 }*/,
					Pair<UInt8, UInt8> _ArrayLayerRange /* = { 0,0 }*/,
					const FVulkanComponentMapping& _ComponentMapping/* = {}*/) const
	{
		//[TODO]: Check more ImageView validity.
		VE_ASSERT(IsOrderedPair(_MipmapLevelRange) && IsOrderedPair(_ArrayLayerRange));
		VE_ASSERT(Type != EImageType::Undefined && "Cannot create the image view for an undefined image!");

		auto NewImageView = CreateSharedPtr<FVulkanImageView>(shared_from_this());
		NewImageView->TypeView   = (_Type   == EImageViewType::Auto)? EImageViewType(this->Type) : _Type;
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
			&NewImageView->Handle) != VK_SUCCESS)
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
		NewBuffer->Usages		= _Usages;
		NewBuffer->SharingMode	= _SharingMode;
		NewBuffer->Location		= _Location;

		VkBufferCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0x0,
			.size  = _Size,
			.usage = AutoCast(NewBuffer->Usages),
			.sharingMode = AutoCast(NewBuffer->SharingMode),
			.queueFamilyIndexCount	= 0,
			.pQueueFamilyIndices	= nullptr,
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
		if (!IsReleased())
		{ Free(&Handle, &Allocation); }
	}

	SharedPtr<FVulkanBuffer> FVulkanBuffer::
	Clone() const
	{
		return GVulkan->Allocator->CreateBuffer(
			GetSize(),
			Usages,
			SharingMode,
			Location);
	}

	SharedPtr<FVulkanImage> FVulkanImage::
	Clone() const
	{
		return GVulkan->Allocator->CreateImage(
			Type,
			Extent,
			Format,
			Aspects,
			Usages,
			Tiling,
			SampleRate,
			MipmapLevels,
			ArrayLayers,
			SharingMode,
			Location);
	}

} } // namespace VE::Runtime