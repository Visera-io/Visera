module;
#include <Visera.h>
export module Visera.Core.Media;
export import Visera.Core.Media.Image;
export import Visera.Core.Media.Model;

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
		//[TODO]: Remove
		VE_API CreateImage(FName _Name, const FPath& _Path)->SharedPtr<FImage>;
		VE_API CreateImage(FName _Name, UInt32 _Width, UInt32 _Height, UInt32 _BPP = 4*8)->SharedPtr<FImage>;

		VE_API SearchImage(FName _Name) -> SharedPtr<FImage>;

		VE_API CreateModel(FName _Name, const FPath& _Path)->SharedPtr<FModel>;

	private:
		static inline FRWLock RWLock;
		static inline HashMap<FName, SharedPtr<FImage>> ImageTable; //[TODO]: MediaTable
		static inline HashMap<FName, SharedPtr<FModel>> ModelTable; //[TODO]: MediaTable
	};

	SharedPtr<FImage> Media::
	CreateImage(FName _Name, const FPath& _Path)
	{
		SharedPtr<FImage> Result;

		RWLock.StartWriting();
		{
			auto& ImageSlot = ImageTable[_Name];
			if (ImageSlot == nullptr)
			{ 
				ImageSlot =  CreateSharedPtr<FImage>(_Path);
				Result    = ImageSlot;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	SharedPtr<FImage> Media::
	CreateImage(FName _Name, UInt32 _Width, UInt32 _Height, UInt32 _BPP /* = 4*8*/ )
	{
		SharedPtr<FImage> Result;

		RWLock.StartWriting();
		{
			auto& ImageSlot = ImageTable[_Name];
			if (ImageSlot == nullptr)
			{
				ImageSlot =  CreateSharedPtr<FImage>(_Width, _Height, _BPP);
				Result    = ImageSlot;
			}
		}
		RWLock.StopWriting();

		return Result;
	}

	SharedPtr<FImage> Media::
	SearchImage(FName _Name)
	{
		SharedPtr<FImage> Result;

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
	CreateModel(FName _Name, const FPath& _Path)
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