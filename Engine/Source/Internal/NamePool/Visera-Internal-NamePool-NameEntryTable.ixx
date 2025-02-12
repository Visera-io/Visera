module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameEntryTable;

import :Common;

import Visera.Core.System.Memory;
import Visera.Core.System.Concurrency;
import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE { namespace Internal
{

	class FNameEntryTable
	{
	public:
		enum
		{
			SectionByteSize		= 256 * OneKByte,				//Unaligned Size

			MaxSectionBits		= 13,
			MaxSections			= (1U << MaxSectionBits),		// 8192 Sections
			NameEntryAlignment	= alignof(FNameEntry),
			NameEntryByteStride = NameEntryAlignment,

			SectionOffsetBits	= 16, // Since NameEntryAlignment and NameEntryByteStride are same (2), we can use 16bits to encode 65536 offsets (remeber these factor!)
			MaxSectionOffset	= (1U << SectionOffsetBits), //65536 Offsets (Max)
			NameEntryHandleBits	= MaxSectionBits + SectionOffsetBits,
			NameEntryHandleMask	= (1U << NameEntryHandleBits) - 1,
		};
		auto Insert(StringView _ParsedName, const FNameHash& _NameHash) -> FNameEntryHandle;
		auto LookUp(FNameEntryHandle _NameEntryHandle) const -> const FNameEntry&
		{ // Reset not needed since any valid NameEntryHandle is distributed after the insertion.
		  return *reinterpret_cast<FNameEntry*>(Sections[_NameEntryHandle.GetSectionIndex()].Data + NameEntryByteStride * _NameEntryHandle.GetSectionOffset());
		}

	private:
		struct FNameEntryTableSection
		{
			Byte*	Data; // Entries
			UInt32	CurrentByteCursor = 0;  //Point at the current entry.
		};
		mutable FRWLock RWLock;
		Int32 CurrentSectionCursor = -1;
		Segment<FNameEntryTableSection, MaxSections> Sections;

		void AllocateNewSection();

	public:
		FNameEntryTable();
		~FNameEntryTable();
	};

	FNameEntryHandle FNameEntryTable::
	Insert(StringView _ParsedName, const FNameHash& _NameHash)
	{
		UInt32 AlignedMemorySize = Memory::Align(Memory::GetDataOffset(&FNameEntry::ANSIName) + _ParsedName.size(), NameEntryAlignment);
		VE_ASSERT(AlignedMemorySize <= SectionByteSize);

		FNameEntryHandle NameEntryHandle;

		RWLock.StartWriting();
		{
			if (Sections[CurrentSectionCursor].CurrentByteCursor + AlignedMemorySize >= SectionByteSize)
			{ AllocateNewSection(); }

			VE_ASSERT(CurrentSectionCursor % NameEntryByteStride == 0 &&
					  CurrentSectionCursor / NameEntryByteStride < MaxSectionOffset);

			auto& CurrentSection = Sections[CurrentSectionCursor];
			NameEntryHandle = FNameEntryHandle{ static_cast<UInt32>(CurrentSectionCursor),
												CurrentSection.CurrentByteCursor / NameEntryByteStride };

			CurrentSection.CurrentByteCursor += AlignedMemorySize;
		}
		RWLock.StopWriting();

		auto& NewNameEntry  = const_cast<FNameEntry&>(LookUp(NameEntryHandle));
		NewNameEntry.Header.LowerCaseProbeHash = _NameHash.GetLowerCaseProbeHash();
		NewNameEntry.Header.Size			   = _ParsedName.size();
		Memory::Memcpy(NewNameEntry.ANSIName, _ParsedName.data(), NewNameEntry.Header.Size);

		return NameEntryHandle;
	}

	void FNameEntryTable::
	AllocateNewSection()
    {
		if (CurrentSectionCursor >= MaxSections)
		{ throw SRuntimeError(Text("NamePool's MemoryBlocks have reached the maximum limit {}!", UInt32(MaxSections))); }

			
		if (CurrentSectionCursor >= 0)
		{
			auto& CurrentSection = Sections[CurrentSectionCursor];
			// Solve Special Case: Terminator
			if (CurrentSection.CurrentByteCursor + Memory::GetDataOffset(&FNameEntry::ANSIName) <= SectionByteSize)
			{
				FNameEntry* Terminator = (FNameEntry*)(Sections[CurrentSectionCursor].Data + CurrentSection.CurrentByteCursor);
				Terminator->Header.Size = 0;
			}
		}

		Log::Debug("Allocating a new FNameEntryTable Section at index({}).", CurrentSectionCursor + 1);
		auto& NewSection = Sections[++CurrentSectionCursor];
		NewSection.Data = (Byte*)Memory::MallocNow(SectionByteSize, NameEntryAlignment);
		NewSection.CurrentByteCursor = 0;
    }

	FNameEntryTable::
	FNameEntryTable()
	{
		AllocateNewSection();
	}

	FNameEntryTable::
	~FNameEntryTable()
	{
		for (auto& Section : Sections)
		{
			Memory::Free(Section.Data, NameEntryAlignment);
			Section.CurrentByteCursor = 0;
		}
		CurrentSectionCursor = -1;
	}

} } // namespace VE::Internal