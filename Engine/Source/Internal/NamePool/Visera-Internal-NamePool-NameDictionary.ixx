module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameDictionary;

export import :NameEntry;

import Visera.Core.System;
import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE { namespace Internal
{
	class FNamePool;

	class FNameDictionary
	{
		friend class FNamePool;
	public:
		enum
		{
			PartitionSize		= 256 * OneKByte,					//Unaligned Size

			MaxPartitionBits	= 13,
			MaxPartitions		= 1U << MaxPartitionBits,		// 8192 Partitions
			NameEntryAlignment	= alignof(FNameEntry),
			NameEntryByteStride = NameEntryAlignment,
			PartitionOffsetBits	= 16, // Since NameEntryAlignment and NameEntryByteStride are same (2), we can use 16bits to encode 65536 offsets (remeber these factor!)
			PartitionOffsets	= 1U << PartitionOffsetBits,	//65536 Offsets (Max)
			NameEntryIDBits		= MaxPartitionBits + PartitionOffsetBits,
			NameEntryIDMask		= (1U << NameEntryIDBits) - 1,
		};

		struct FBookMark //Partitions[PartitionID].Entries[EntryIndex]
		{
			UInt32 PartitionID = 0;
			UInt32 EntryOffset = 0; // Note that this offset is divided by NameEntryByteStride
			static inline auto
			CreateFromNameEntryID(FNameEntryID _NameEntryID) -> FBookMark { return FBookMark{ .PartitionID = _NameEntryID.Value >> PartitionOffsetBits, .EntryOffset = _NameEntryID.Value & (PartitionOffsets - 1) }; }
			auto inline
			ToNameEntryID() const -> FNameEntryID { FNameEntryID ID{}; ID.Value = PartitionID << PartitionOffsetBits | EntryOffset; return ID; }
			operator FNameEntryID() const { return ToNameEntryID(); }
		};

		auto LookUp(FBookMark _BookMark) const -> const FNameEntry&
		{
			// Lock not needed since any valid BookMark is distributed after the insertion.
			return *reinterpret_cast<FNameEntry*>(Partitions[_BookMark.PartitionID].Data + NameEntryByteStride * _BookMark.EntryOffset);
		}

	private:

		struct FNameEntryList
		{
			Byte*	Data; // Entries
			UInt32	CurrentByteCursor = 0;  //Point at the current entry.
		};

		mutable FRWLock RWLock;
		Segment<FNameEntryList, MaxPartitions> Partitions;
		Int32 CurrentPartitionCursor = -1;

		auto Insert(FNameEntry::FHeader _Header, StringView _Name) -> FBookMark
		{
			VE_ASSERT(_Header.Length == _Name.length());

			Log::Debug("Inserting a new FNameEntry \"{}\"", _Name);
			VE_ASSERT(False);//[FIXME]: Memcpy bug.
			UInt32 AlignedMemorySize = Memory::Align((_Name.length() + 1) * sizeof(ANSIChar), NameEntryAlignment);
			VE_ASSERT(AlignedMemorySize <= PartitionSize);
			
			FBookMark BookMark;

			RWLock.StartWriting();
			{
				if (Partitions[CurrentPartitionCursor].CurrentByteCursor + AlignedMemorySize >= PartitionSize)
				{ AllocateNewPartition(); }

				VE_ASSERT(CurrentPartitionCursor % NameEntryByteStride == 0 &&
						  CurrentPartitionCursor / NameEntryByteStride < PartitionOffsets);

				auto& CurrentPartition = Partitions[CurrentPartitionCursor];
				BookMark.PartitionID = CurrentPartitionCursor;
				BookMark.EntryOffset = CurrentPartition.CurrentByteCursor / NameEntryByteStride;

				CurrentPartition.CurrentByteCursor += AlignedMemorySize;
			}
			RWLock.StopWriting();

			auto& NewNameEntry  = const_cast<FNameEntry&>(LookUp(BookMark));
			NewNameEntry.Header = _Header;
			Memory::Memcpy(NewNameEntry.ANSIName, _Name.data(), AlignedMemorySize);

			return BookMark;
		}

		FNameDictionary()
		{
			AllocateNewPartition();
		}

		~FNameDictionary()
		{
			for (auto& Partition : Partitions)
			{
				Memory::Free(Partition.Data, NameEntryAlignment);
				Partition.CurrentByteCursor = 0;
			}
			CurrentPartitionCursor = -1;
		}

		void AllocateNewPartition()
        {
			if (CurrentPartitionCursor >= MaxPartitions)
			{ throw SRuntimeError(Text("NamePool's MemoryBlocks have reached the maximum limit {}!", UInt32(MaxPartitions))); }

			
			if (CurrentPartitionCursor >= 0)
			{
				auto& CurrentPartition = Partitions[CurrentPartitionCursor];
				// Solve Special Case: Terminator
				if (CurrentPartition.CurrentByteCursor + Memory::GetDataOffset(&FNameEntry::ANSIName) <= PartitionSize)
				{
					FNameEntry* Terminator = (FNameEntry*)(Partitions[CurrentPartitionCursor].Data + CurrentPartition.CurrentByteCursor);
					Terminator->Header.Length = 0;
				}
			}

			Log::Debug("Allocating a new FNameDictionary partition at index({}).", CurrentPartitionCursor + 1);
			auto& NewPartition = Partitions[++CurrentPartitionCursor];
			NewPartition.Data = (Byte*)Memory::MallocNow(PartitionSize, NameEntryAlignment);
			NewPartition.CurrentByteCursor = 0;
        }
	};

} } // namespace VE::Internal