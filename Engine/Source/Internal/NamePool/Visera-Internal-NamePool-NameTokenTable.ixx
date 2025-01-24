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
	public:
        enum
        {
            MaxNameTokenTableSectionsBits    = 8,
            MaxNameTokenTableSections        = (1U << MaxNameTokenTableSectionsBits),
            MaxNameTokenTableSectionsMask    = MaxNameTokenTableSections - 1,
        };
		auto Insert(StringView _ParsedName, FNameHash _NameHash) -> FNameEntryHandle;

        FNameTokenTable() = delete;
        FNameTokenTable(FNameEntryTable* _NameEntryTable) : LinkedNameEntryTable{ _NameEntryTable } {}

	private: 
		class FNameTokenTableSection
		{
            friend class FNameTokenTable;
            enum
            {
                InitNameTokens    = 1 << 8,
                InitSectionSize   = InitNameTokens * sizeof(FNameToken), // Unaligned
            };
            static constexpr float GrowingThresh = 0.9;
		public:
			auto ProbeToken(FNameHash _NameHash, std::function<Bool(FNameToken)> _Prediction) const -> const FNameToken&;
			auto ClaimToken(const FNameToken* _Token, FNameToken _Value) { 
                VE_ASSERT(RWLock.IsLocked() && !_Token->IsClaimed()); 
                const_cast<FNameToken*>(_Token)->HashAndID = _Value.HashAndID; 
                ++UseCount; 
            };
			Bool ShouldGrow() const { return UseCount > Capacity * GrowingThresh; }

            FNameTokenTableSection();
            ~FNameTokenTableSection();

		private:
            mutable FRWLock RWLock;
			FNameToken* Tokens;
			UInt32      Capacity = 0;
			UInt32      UseCount = 0;
		};

        FNameEntryTable* const LinkedNameEntryTable;
		Segment<FNameTokenTableSection, MaxNameTokenTableSections> Sections;
        void GrowAndRehash(FNameTokenTableSection& _Section);
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

    FNameEntryHandle FNameTokenTable::
    Insert(StringView _ParsedName, FNameHash _NameHash)
    {
        auto& Section = Sections[_NameHash.GetTokenTableSectionIndex()];
        FNameEntryHandle NewNameEntryHandle{};

        Section.RWLock.StartWriting();
        {
            if (Section.ShouldGrow()) { GrowAndRehash(Section); }
            auto& Token = Section.ProbeToken(_NameHash,
                                             /*Further Comparsion*/
                                             [&](FNameToken _Token)->Bool
                                             { 
                                                 VE_ASSERT(_Token.IsClaimed());
                                                 FNameEntryHandle NameEntryHandle{_Token};
                                                 auto& NameEntry = LinkedNameEntryTable->LookUp(NameEntryHandle);
                                                 if (NameEntry.GetHeader().LowerCaseProbeHash == _NameHash.GetLowerCaseProbeHash())
                                                 {
                                                     if (NameEntry.GetANSIName() == _ParsedName)
                                                     {
                                                         return True; // [O]
                                                     }
                                                     return False; // [X]: Different String Literal
                                                 }
                                                 return False; // [X]: Different LowerCase Probe Hash
                                             });
            
            if (!Token.IsClaimed())
            {
                NewNameEntryHandle = LinkedNameEntryTable->Insert(_ParsedName, _NameHash);
                Section.ClaimToken(&Token, FNameToken{ NewNameEntryHandle, _NameHash });
            }
            else
            {
                NewNameEntryHandle = FNameEntryHandle{ Token };
            }
        }
        Section.RWLock.StopWriting();

        return NewNameEntryHandle;
    }

	void FNameTokenTable::
    GrowAndRehash(FNameTokenTableSection& _Section)
    {
        VE_ASSERT(_Section.RWLock.IsLocked());

        //Log::Debug("Growing FNamePool::FNameSlot...");
        auto* OldTokens     = _Section.Tokens;
        auto  OldCapacity   = _Section.Capacity;

        // Reallocating
        _Section.Capacity  <<= 1; // Double
        _Section.UseCount  = 0;
        _Section.Tokens    = (FNameToken*)Memory::MallocNow(_Section.Capacity * sizeof(FNameToken), alignof(FNameToken));

        // Rehashing
        for (UInt32 It = 0; It < OldCapacity; ++It)
        {   
            FNameToken& CurrentOldToken = *(OldTokens + It);
            if (CurrentOldToken.IsClaimed())
            {
                auto& NameEntry = LinkedNameEntryTable->LookUp(FNameEntryHandle{ CurrentOldToken });
                FNameHash NameHash{ NameEntry.GetANSIName() };

                auto& Token = _Section.ProbeToken(NameHash, [](FNameToken _Token) -> Bool { return False; });
                VE_ASSERT(!Token.IsClaimed() && "Any Token in the old section is UNIQUE!");
                _Section.ClaimToken(&Token, CurrentOldToken);
            }
        }
        Memory::Free(OldTokens, alignof(FNameToken));
    }

    const FNameToken& FNameTokenTable::FNameTokenTableSection::
    ProbeToken(FNameHash _NameHash, std::function<Bool(FNameToken)> _Prediction) const
    {
        VE_ASSERT(UseCount <= Capacity && "Curreny FNamePoolShard is full! Try to call FNamePool::GrowAndRehash(...)");
        VE_ASSERT(IsPowerOfTwo(Capacity));
         
        UInt32 TokenIndexMask = Capacity - 1;
        for(UInt32 Offset = 0; True; Offset++)
        {
            auto& Token = Tokens[(_NameHash.GetUnmaskedTokenIndex() + Offset) & TokenIndexMask];
            if (!Token.IsClaimed() || _Prediction(Token))
            {
                return Token; // Call FNameTokenTable::ClaimToken(...) if it should be used.
            }
        }
        VE_ASSERT(False && "Absolutely return a Token!");
    }

} } // namespace VE::Internal