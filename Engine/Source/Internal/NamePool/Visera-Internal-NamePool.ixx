module;
#include <Visera.h>
export module Visera.Internal.NamePool;

import :NameEntry;
import :NameSlot;

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
        enum//Settings (!CRITICAL!)
        { 
            //NamePool
            InitNamePoolShards   = 256,
            //NameSlot
            MaxNameSlots         = 256,  //Sub-Effects:{Register}
            MaxNameDigitCount    = 10,   //Sub-Effects:...
            MaxNameNumber        = INT32_MAX,
            //MemoryBlock
            MaxMemoryBlocks      = 4096, //Sub-Effects:...
            MaxMemoryBlockSize   = 256 * OneKByte,
            MemoryBlockAlignment = 2,
        };

    public:
        auto Register(const String& _Name) throw (SRuntimeError) -> Tuple<UInt32, UInt32>;
        auto Search(StringView _Name) -> const char* { return nullptr; }

    private:
        mutable FRWLock RWLock;

        struct FShard
        {
            FNameSlot* Slots = nullptr;
        };
        Array<FShard> Shards;

        struct FMemoryBlock
        {
            UInt32 ByteCursor  = 0;  //Point at the current byte.
            Byte*  Data;
        };
        Segment<FMemoryBlock, MaxMemoryBlocks> MemoryBlocks;
        UInt32 MemoryBlockCursor = 0; // Point at the current memory block.

    public:
        FNamePool();
        ~FNamePool();

    private:
        void Grow()
        {
            Log::Warn("Growing NamePool...");
        }

        /*<< Helpers >>*/
        Tuple<Int32, UInt32> //[Number(<0 means invalid), NameLength]
        ParseName(const char* _Name, UInt32 _Length)
        {
            constexpr auto IsDigit = [](char _Char)->bool { return '0' <= _Char && _Char <= '9'; };

            UInt32 Digits = 0;
            const char* It = _Name + _Length - 1;
            //Count Digits
            while(It >= _Name && IsDigit(*It)) { ++Digits; --It; }
            //Convert Case
            while (It >= _Name) { std::tolower(static_cast<unsigned char>(*It)); --It; }       
            
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
                        { return { Number , _Length - Digits - 1 }; }
                    }
                }
                return { -1, _Length }; //Invalid Name
            }
            else return { 0, _Length }; //No Numbers
        }
    };

    Tuple<UInt32, UInt32> FNamePool::
    Register(const String& _Name)
    {
        auto [Number, NameLength] = ParseName(_Name.data(), _Name.size());
        if (Number < 0) { throw SRuntimeError("Ref: Naming Convention"); }

        String Name = _Name.substr(0, NameLength);

        union FNameHash
        {
            UInt64 Value;
        };
        FNameHash NameHash{ .Value = Hash::CityHash64(Name) };
        VE_ASSERT(MaxNameSlots == 256 && "If you use a new size, make sure that the encoding is revised.");


        RWLock.StartWriting();
        {

        }
        RWLock.StopWriting();

        return { 1, Number + 1 };
    }

    FNamePool::
    FNamePool()
    {
        //Initialize Shards
        Shards.resize(InitNamePoolShards);

        UInt64 ShardSize = MaxNameSlots * sizeof(FNameSlot);
        for (auto& Shard : Shards)
        {
            Shard.Slots = (FNameSlot*)Memory::Malloc(ShardSize, alignof(FNameSlot));
            Memory::Memset(Shard.Slots, 0, ShardSize);
        }
    }

    FNamePool::
    ~FNamePool()
    {
        for (auto& Shard : Shards)
        {
            Memory::Free(Shard.Slots, alignof(FNameSlot));
        }
    }

} } // namespace VE::Internal