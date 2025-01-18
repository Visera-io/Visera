module;
#include <Visera.h>
export module Visera.Internal.NamePool;
import :NameDictionary;

import Visera.Core.Math.Hash;
import Visera.Core.Signal;
import Visera.Core.Log;
import Visera.Core.System;

export namespace VE { namespace Internal
{
    enum class EName : UInt32
	{
		None = 0, //MUST be registered at first for assuring FNameEntryID == 0 && Number == 0

		MaxReservedName,
	};

    class FNamePool
    {
        enum // Config
        {
            MaxNamePoolShardsBits   = 8,
            MaxNamePoolShards       = 1U << MaxNamePoolShardsBits,
            MaxNamePoolShardsMask   = MaxNamePoolShards - 1,
            InitNameSlotsPerShard   = 1 << 8, //Sub-Effects:{Grow(FPoolShard*)}
            NamePoolShardGrowThresh = 90/*%*/, //Sub-Effects:{Grow(FPoolShard*)}]

            MaxNameDigitCount   = 10,  //Sub-Effects:...
            MaxNameNumber       = INT32_MAX,

            /*L:[ 0~31]: Slot Index*/
            UnmaskedSlotIndexMask  = 1U << 32 - 1,
            /*H:[32~39]: Shard Index*/
            ShardIndexBits = 8,
            ShardIndexMask = 1U << ShardIndexBits - 1,
            /*H:[40~45]: Lower Case Probe Hash*/
            LowerCaseProbeHashBits = FNameEntry::LowerCaseProbeHashBits,
            LowerCaseProbeHashMask = 1U << LowerCaseProbeHashBits - 1,
        };
    public:
        auto Register(const ANSIChar* _Name, UInt64 _Length) throw (SRuntimeError) -> ResultPackage<UInt32, UInt32>; //[Handle_, Number_]
        auto Search(UInt32 _Handle) const -> StringView;

    private:
        struct FNamePoolShard
        {
            friend class FNamePool;

            class FNameSlot
	        {
                friend class FNamePool;
		        friend class FNamePoolShard;

		        enum : EnumMask
		        {
			        EntryIDBits  = 29U, //OntoMapping{FNameEntryID.Value:29}
			        EntryIDMask  = 1U << EntryIDBits - 1U,
                    ProbeHashBits    = 3U,
                    ProbeHashOffsets = EntryIDBits,
			        ProbeHashMask    = ~EntryIDMask,
		        };
		
		        UInt32 HashAndID = 0U;
		        auto GetID()		const -> UInt32 { return HashAndID & EntryIDMask; }
		        auto GetProbeHash()	const -> UInt32 { return HashAndID & ProbeHashMask; }
		        Bool IsUsed()		const { return !!HashAndID; }
	        };

            auto ProbeSlot(UInt32 _Lower32Bits, std::function<Bool(const FNameSlot& _Slot)> _Prediction = [](const FNameSlot& _Slot) -> Bool { return False; }) const -> FNameSlot&;
            auto ClaimSlot(FNameSlot* _Slot, UInt32 _HashAndID) { VE_ASSERT(RWLock.IsLocked() && !_Slot->IsUsed()); _Slot->HashAndID = _HashAndID; ++UseCount; };
            Bool ShouldGrow() const { return UseCount > Capacity * (NamePoolShardGrowThresh / 100.0); }

            FNameSlot* Slots;
            UInt32     Capacity = 0;
            UInt32     UseCount = 0;
            mutable FRWLock RWLock;

            FNamePoolShard();
            ~FNamePoolShard();
        };
        Segment<FNamePoolShard, MaxNamePoolShards> Shards; // Name Slot Allocator & Manager
        FNameDictionary NameDictionary;                    // Physical Memory Allocator & Manager

    private:
        //[Number(<0 means invalid), NameLength]
        auto ParseName(const char* _Name, UInt32 _Length) const -> ResultPackage<Int32, UInt32>;
        void GrowAndRehash(FNamePoolShard* _TargetShard, const FNameDictionary& _Dictionary);
        
    public:
        FNamePool();
        ~FNamePool();
    };

    ResultPackage<UInt32, UInt32> FNamePool::
    Register(const ANSIChar* _Name, UInt64 _Length)
    {
        //Log::Debug("Registering a new FName:{}", _Name);

        auto [Number, NameLength] = ParseName(_Name, _Length);
        StringView PureName{ _Name, NameLength };
        Log::Debug("Parsed FName: {}({})", PureName, Number);
        if (Number < 0) { throw SRuntimeError("Bad Name! -- Naming Convention:([#Name][_#Number]?)."); }

        UInt64 NameHash = Hash::CityHash64(_Name, NameLength);
        UInt32 Lower32Bits  = NameHash & (~0U);
        UInt32 Higher32Bits = NameHash >> 32;

        // Start Registering
        UInt32 UnsolvedSlotIndex  = Lower32Bits  & UnmaskedSlotIndexMask;
        UInt32 TargetShardIndex   = Higher32Bits & ShardIndexMask; VE_ASSERT(TargetShardIndex < MaxNamePoolShards);
        UInt32 LowerCaseProbeHash = Higher32Bits & LowerCaseProbeHashMask;                      // Accelerate String Comparsion
        UInt32 SlotProbeHash      = Higher32Bits & FNamePoolShard::FNameSlot::ProbeHashMask;    // Accelerate Slot Probing

        FNameEntry NewNameEntry{};
        NewNameEntry.Header = FNameEntry::FHeader{ .LowerCaseProbeHash = UInt16(LowerCaseProbeHash), .Length = UInt16(NameLength) };
        auto BookMark = NameDictionary.Insert(NewNameEntry.Header, PureName);

        auto& TargetShard = Shards[TargetShardIndex];
        UInt32 HashAndID = 0;
        TargetShard.RWLock.StartWriting();
        {
            if (TargetShard.ShouldGrow())
            { GrowAndRehash(&TargetShard, NameDictionary); }

            auto& Slot = TargetShard.ProbeSlot(UnsolvedSlotIndex);
            HashAndID = SlotProbeHash | FNameEntryID(BookMark).GetID();
            TargetShard.ClaimSlot(&Slot, HashAndID);
        }
        TargetShard.RWLock.StopWriting();

        return { HashAndID, Number }; // Numer == 0 means NO Number
    }

    StringView FNamePool::
    Search(UInt32 _Handle) const
    {
        FNameEntryID EntryID{ _Handle };
        auto& NameEntry = NameDictionary.LookUp(FNameDictionary::FBookMark::CreateFromNameEntryID(EntryID));
        if (NameEntry.Header.Length == 0) { return ""; }
        return StringView(NameEntry.ANSIName, NameEntry.Header.Length);
    }

    FNamePool::FNamePoolShard::FNameSlot& FNamePool::FNamePoolShard::
    ProbeSlot(UInt32 _UnmaskedSlotIndex/*(Lower32Bits)*/, std::function<Bool(const FNameSlot& _Slot)> _Prediction/* = []() -> Bool { return False; }*/) const
    {
        VE_ASSERT(UseCount <= Capacity && "Curreny FNamePoolShard is full! Try to call FNamePool::GrowAndRehash(...)");

        VE_ASSERT(IsPowerOfTwo(Capacity));
        UInt32 SlotIndexMask = Capacity - 1;
        for(UInt32 Offset = 0; True; Offset++)
        {
            auto& Slot = Slots[(_UnmaskedSlotIndex + Offset) & SlotIndexMask];
            if (!Slot.IsUsed() || _Prediction(Slot))
            {
                return Slot; // Call FNamePoolShard::ClaimSlot(Slot) if it is used.
            }
        }
    }

    void FNamePool::
    GrowAndRehash(FNamePoolShard* _TargetShard, const FNameDictionary& _Dictionary)
    {
        VE_ASSERT(_TargetShard->RWLock.IsLocked());

        Log::Debug("Growing FNamePool::FNameSlot...");
        auto* OldSlots      = _TargetShard->Slots;
        auto  OldCapacity   = _TargetShard->Capacity;

        // Reallocating
        _TargetShard->Capacity <<= 1; // Double
        _TargetShard->UseCount  = 0;
        _TargetShard->Slots     = (FNamePoolShard::FNameSlot*)Memory::MallocNow(_TargetShard->Capacity * sizeof(FNamePoolShard::FNameSlot), alignof(FNamePoolShard::FNameSlot));

        // Rehashing
        for (UInt32 It = 0; It < OldCapacity; ++It)
        {   
            auto& CurrentOldSlot = *(OldSlots + It);
            if (CurrentOldSlot.IsUsed())
            {
                auto& NameEntry = _Dictionary.LookUp({ FNameEntryID{CurrentOldSlot.HashAndID} });
                auto HashValue = Hash::CityHash64(NameEntry.ANSIName, NameEntry.GetSizeWithoutTerminator());
                UInt32 UnmarkedSlotIndex = HashValue & UnmaskedSlotIndexMask;
                auto& FreeSlot = _TargetShard->ProbeSlot(UnmarkedSlotIndex);
                _TargetShard->ClaimSlot(&FreeSlot, CurrentOldSlot.HashAndID);
            }
        }

        Memory::Free(OldSlots, alignof(FNamePoolShard::FNameSlot));
    }

    FNamePool::
    FNamePool()
    {
        
    }

    FNamePool::
    ~FNamePool()
    {
        
    }

    FNamePool::FNamePoolShard::
    FNamePoolShard()
    {
        UInt64 SlotsSizePerShard = InitNameSlotsPerShard * sizeof(FNameSlot);
        Slots = (FNameSlot*)Memory::MallocNow(SlotsSizePerShard, alignof(FNameSlot));
        Capacity = InitNameSlotsPerShard;
        UseCount = 0;
    }

    FNamePool::FNamePoolShard::
    ~FNamePoolShard()
    {
        Memory::Free(Slots, alignof(FNameSlot));
        Capacity = 0;
        UseCount = 0;
    }

    ResultPackage<Int32, UInt32> FNamePool::
    ParseName(const char* _Name, UInt32 _Length) const
    {
        constexpr auto IsDigit = [](char _Char)->bool { return '0' <= _Char && _Char <= '9'; };

        UInt32 Digits = 0;
        char* It = const_cast<char*>(_Name) + _Length - 1;
        //Count Digits
        while(It >= _Name && IsDigit(*It)) { ++Digits; --It; }
        //Convert Case
        while (It >= _Name) { *It = std::tolower(static_cast<unsigned char>(*It)); --It; }       
            
        if (Digits)
        {
            UInt32 FirstDigitCursor = _Length - Digits;
                
            if (/*Valid Digit Length*/      (Digits < _Length) &&
                /*Valid Naming Convention*/ (_Name[FirstDigitCursor - 1] == '_') &&
                /*Valid Digit Count*/       (Digits <= MaxNameDigitCount))
            {
                if (/*Name_0 is Valid*/         Digits == 1 ||
                    /*Zero Prefix is InValid*/  _Name[FirstDigitCursor] != '0')
                {
                    Int32 Number = atoi(_Name + FirstDigitCursor);
                    if (Number < MaxNameNumber)
                    { return { Number + 1, _Length - Digits - 1 }; }
                }
            }
            return { -1, _Length }; //Invalid Name
        }
        else return { 0, _Length }; //No Numbers
    }

} } // namespace VE::Internal