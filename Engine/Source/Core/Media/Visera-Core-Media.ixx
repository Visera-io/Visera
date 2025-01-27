module;
#include <Visera.h>
export module Visera.Core.Media;
import Visera.Core.Media.Image;
import Visera.Core.Type;
import Visera.Core.System.FileSystem;
import Visera.Core.System.Concurrency;

export namespace VE
{

	class Media
	{
		VE_MODULE_MANAGER_CLASS(Media);
	public:
		VE_API CreateImage(FName _Name, const FPath& _Path)->SharedPtr<FImage>;
		VE_API SearchImage(FName _Name) -> SharedPtr<FImage>;

	private:
		static inline FRWLock RWLock;
		static inline HashMap<FName, SharedPtr<FImage>> ImageTable; //[TODO]: MediaTable
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
				ImageSlot =  CreateSharedPtr<FImage>(_Name, _Path);
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

} // namespace VE