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
        enum//Settings (!!!CRITICAL!!!)
        { 
            //NamePool
            MaxNamePoolShards    = 256, //8bits encoded
            InitNamePoolShardSize= 256, //Sub-Effects:{Register}
            //Name
            MaxNameLength        = 1024 * OneByte,
            MaxNameDigitCount    = 10,  //Sub-Effects:...
            MaxNameNumber        = INT32_MAX,
            //MemoryBlock
            MaxMemoryBlocks      = 4096, //Sub-Effects:...
            MaxMemoryBlockSize   = 256 * OneKByte,
            MaxMemoryBlockUsage  = UInt32(MaxMemoryBlockSize * 0.95), //Sub-Effects:{AllocateNewMemoryBlock}
            MemoryBlockAlignment = 2,
            MaxMemoryUsage       = MaxMemoryBlocks * MaxMemoryBlockSize, //1GB
        };

    public:
        auto Register(String& _Name) throw (SRuntimeError) -> Tuple<UInt32, UInt32>;

    private:
        mutable FRWLock RWLock;
        struct FPoolShard
        {
            FNameSlot* Slots;
            UInt32     SlotCursor = 0;
        };
        Array<FPoolShard> Shards;

        struct FMemoryBlock
        {
            UInt32 ByteCursor  = 0;  //Point at the current byte.
            Byte*  Data;
        };
        Segment<FMemoryBlock, MaxMemoryBlocks> MemoryBlocks;
        Int32 MemoryBlockCursor = -1; // Point at the current memory block.

    public:
        FNamePool();
        ~FNamePool();

    private:
        void AllocateNewMemoryBlock()
        {
            if (MemoryBlockCursor >= 0)
            { Log::Debug("Allocating a new NamePool MemoryBlock (Last Memory Usage Rate: {}%)...", 100 * MemoryBlocks[MemoryBlockCursor].ByteCursor / MaxMemoryBlockSize); }
            
            if (MemoryBlockCursor < MaxMemoryBlocks)
            {
                auto& NewMemoryBlock = MemoryBlocks[++MemoryBlockCursor];
                NewMemoryBlock.Data = (Byte*)Memory::MallocNow(MaxMemoryBlockSize, MemoryBlockAlignment);
            }
            else throw SRuntimeError("NamePool's MemoryBlocks have reached the maximum limit!");
        }

        /*<< Helpers >>*/
        Tuple<Int32, UInt32> //[Number(<0 means invalid), NameLength]
        ParseName(const char* _Name, UInt32 _Length)
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
                        { return { Number , _Length - Digits - 1 }; }
                    }
                }
                return { -1, _Length }; //Invalid Name
            }
            else return { 0, _Length }; //No Numbers
        }
    };

    Tuple<UInt32, UInt32> FNamePool::
    Register(String& _Name)
    {
        auto [Number, NameLength] = ParseName(_Name.data(), _Name.size());
        if (Number < 0) { throw SRuntimeError("Bad Name! Naming Convention:(#Name_{Number})."); }

        String Name = _Name.substr(0, NameLength);
        UInt64 NameHash = Hash::CityHash64(Name);

        Log::Debug("Registering a new FName:{}", _Name);
        RWLock.StartWriting();
        {

        }
        RWLock.StopWriting();

        return { 1/*[FIXME]*/, Number + 1};
    }

    FNamePool::
    FNamePool()
    {
        //Initialize Shards
        Shards.resize(MaxNamePoolShards);

        UInt64 ShardSize = InitNamePoolShardSize * sizeof(FNameSlot);
        for (auto& Shard : Shards)
        {
            Shard.Slots = (FNameSlot*)Memory::MallocNow(ShardSize, alignof(FNameSlot));
            Shard.SlotCursor = 0;
        }
        AllocateNewMemoryBlock();
    }

    FNamePool::
    ~FNamePool()
    {
        for (auto& Shard : Shards)
        {
            Memory::Free(Shard.Slots, alignof(FNameSlot));
            Shard.SlotCursor = 0;
        }
        for (auto& MemoryBlock : MemoryBlocks)
        {
            Memory::Free(MemoryBlock.Data, MemoryBlockAlignment);
        }
    }

} } // namespace VE::Internal