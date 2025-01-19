module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameTokenTable;

import :Common;
import :NameEntryTable;

import Visera.Core.System;
import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE { namespace Internal
{

	class FNameTokenTable
	{
		enum
        {
            MaxNameTokenTableSectionsBits    = 8,
            MaxNameTokenTableSections        = 1U << MaxNameTokenTableSectionsBits,
            MaxNameTokenTableSectionsMask    = MaxNameTokenTableSections - 1,
        };
	public:
		// !!!Call FNameTokenTable::Insert before FNameEntryTable::Insert!!!
		auto Insert(const FNameEntryTable& _NameEntryTable, const FNameHash& _NameHash) -> FNameTokenHandle;

	private:
		class FNameTokenTableSection
		{
            enum
            {
                InitNameTokens    = 1 << 8,
                InitSectionSize   = InitNameTokens * sizeof(FNameToken), // Unaligned
            };
            static constexpr float GrowingThresh = 0.9;
		public:
			auto ProbeToken(const FNameHash& _NameHash) const -> const FNameToken&;
			auto ClaimToken(FNameToken* _Token, UInt32 _HashAndID) { VE_ASSERT(RWLock.IsLocked() && !_Token->IsClaimed()); _Token->HashAndID = _HashAndID; ++UseCount; };
			Bool ShouldGrow() const { return UseCount > Capacity * GrowingThresh; }
			void GrowAndRehash(const FNameEntryTable& _NameEntryTable);

            FNameTokenTableSection();
            ~FNameTokenTableSection();

		private:
            mutable FRWLock RWLock;
			FNameToken* Tokens;
			UInt32      Capacity = 0;
			UInt32      UseCount = 0;
		};
		Segment<FNameTokenTableSection, MaxNameTokenTableSections> Sections;
	};

    FNameTokenTable::FNameTokenTableSection::
    FNameTokenTableSection()
    {
        Tokens   = (FNameToken*)Memory::MallocNow(InitSectionSize, alignof(FNameToken));
        Capacity = InitNameTokens;
        UseCount = 0;
    }

    FNameTokenTable::FNameTokenTableSection::
    ~FNameTokenTableSection()
    {
        Memory::Free(Tokens, alignof(FNameToken));
        Capacity = 0;
        UseCount = 0;
    }

	void FNameTokenTable::FNameTokenTableSection::
    GrowAndRehash(const FNameEntryTable& _NameEntryTable)
    {
        VE_ASSERT(RWLock.IsLocked());

        //Log::Debug("Growing FNamePool::FNameSlot...");
        auto* OldTokens      = Tokens;
        auto  OldCapacity   = Capacity;

        // Reallocating
        Capacity  <<= 1; // Double
        UseCount  = 0;
        Tokens    = (FNameToken*)Memory::MallocNow(Capacity * sizeof(FNameToken), alignof(FNameToken));

        // Rehashing
        for (UInt32 It = 0; It < OldCapacity; ++It)
        {   
            auto& CurrentOldSlot = *(OldTokens + It);
            if (CurrentOldSlot.IsClaimed())
            {
                VE_ASSERT(False);//WIP
                /*auto&     NameEntry           = _NameEntryTable.LookUp({ FNameEntryID{CurrentOldSlot.HashAndID} });
                FNameHash NameHash{ StringView(NameEntry, NameEntry.GetSizeWithoutTerminator()) };
                auto&     EmptySlot           = ProbeSlot(NameHash.GetUnmaskedTokenIndex());
                ClaimToken(&EmptySlot, CurrentOldSlot.HashAndID);*/
            }
        }
        Memory::Free(OldTokens, alignof(FNameToken));
    }

    
    const FNameToken& FNameTokenTable::FNameTokenTableSection::
    ProbeToken(const FNameHash& _NameHash) const
    {
        VE_ASSERT(UseCount <= Capacity && "Curreny FNamePoolShard is full! Try to call FNamePool::GrowAndRehash(...)");
        VE_ASSERT(IsPowerOfTwo(Capacity));
         
        UInt32 SlotIndexMask = Capacity - 1;
        for(UInt32 Offset = 0; True; Offset++)
        {
            auto& Token = Tokens[(_NameHash.GetUnmaskedTokenIndex() + Offset) & SlotIndexMask];
            if (!Token.IsClaimed() || False)
            {
                VE_ASSERT(False);// WIP
                return Token; // Call FNamePoolShard::ClaimSlot(Slot) if it is used.
            }
        }
        VE_ASSERT(False && "Absolutely return a Token!");
    }

} } // namespace VE::Internal