module;
#include <Visera.h>
export module Visera.Core.Media;
#define VE_MODULE_NAME "Media"
export import Visera.Core.Media.Image;
export import Visera.Core.Media.Model;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.OS.FileSystem;
import Visera.Core.OS.Concurrency;

export namespace VE
{
	using FPixel = FImage::FPixel;

	class Media
	{
		VE_MODULE_MANAGER_CLASS(Media);
	public:
		//[TODO]: Unify Image APIs
		VE_API CreateImage(const FName& _Name, const FPath& _Path)->SharedPtr<IImage>;
		VE_API CreateOldImage(const FName& _Name, UInt32 _Width, UInt32 _Height, UInt32 _BPP = 4*8)->SharedPtr<FImage>;

		VE_API SearchImage(const FName& _Name) -> SharedPtr<IImage>;

		VE_API CreateModel(const FName& _Name, const FPath& _Path)->SharedPtr<FModel>;

	private:
		static inline FRWLock RWLock;
		static inline HashMap<FName, SharedPtr<IImage>> ImageTable;
		static inline HashMap<FName, SharedPtr<FModel>> ModelTable;
	};

	SharedPtr<FPNGImage>
	CreatePNGImage(const FName& _Name, const FPath& _Path)
	{
		return CreateSharedPtr<FPNGImage>(_Path);
	}

	SharedPtr<IImage> Media::
	CreateImage(const FName& _Name, const FPath& _Path)
	{
		SharedPtr<IImage> NewImage;

		if (_Path.GetExtension().ToPlatformString() == ".png")
		{
			RWLock.StartWriting();
			{
				auto& ImageSlot = ImageTable[_Name];
				if (ImageSlot == nullptr)
				{
					ImageSlot = CreateSharedPtr<FPNGImage>(_Path);
					NewImage  = ImageSlot;
				}
				else
				{
					VE_LOG_ERROR("Failed to create image! -- a duplicated image name ({})!", _Name.GetNameWithNumber());
				}
			}
			RWLock.StopWriting();
		}
		else
		{
			VE_LOG_FATAL("Other Image formats WIP...");
		}

		return NewImage;
	}

	SharedPtr<FImage> Media::
	CreateOldImage(const FName& _Name, UInt32 _Width, UInt32 _Height, UInt32 _BPP /* = 4*8*/ )
	{
		VE_LOG_WARN("Creating a new image({}) via an EXPIRED API({})!", _Name.GetNameWithNumber(), __FUNCTION__);
		return CreateSharedPtr<FImage>(_Width, _Height, _BPP);
	}

	SharedPtr<IImage> Media::
	SearchImage(const FName& _Name)
	{
		SharedPtr<IImage> Result;

		RWLock.StartReading();
		{
			auto Target = ImageTable.find(_Name);
			if (Target != ImageTable.end())
			{ Result = Target->second; }
		}
		RWLock.StopReading();

		return Result;
	}

	SharedPtr<FModel> Media::
	CreateModel(const FName& _Name, const FPath& _Path)
	{
		SharedPtr<FModel> Result;

		RWLock.StartWriting();
		{
			auto& ModelSlot = ModelTable[_Name];
			if (ModelSlot == nullptr)
			{
				ModelSlot =  CreateSharedPtr<FModel>(_Path);
				Result    = ModelSlot;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

} // namespace VE