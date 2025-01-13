module;
#include <Visera.h>
export module Visera.Internal.NamePool;

import :NameEntry;
import :NameSlot;

import Visera.Core.Hash;
import Visera.Core.System.Concurrency;

export namespace VE { namespace Internal
{

    class FNamePool
    {
        enum //Settings (!CRITICAL!)
        { 
            MaxMemoryBlocks     = 4096, //Sub-Effects:...
            MaxMemoryBlockSize  = 256 * OneKByte,
            MaxNameSlots        = 256,  //Sub-Effects:{Register}
            MaxNameDigitCount   = 10,   //Sub-Effects:...
            MaxNameNumber       = INT32_MAX,

            //MaxMemoryUsage = MaxMemoryBlocks * MaxNameSlots * OneByte,
        };
    public:
        void Register(const String& _Name);
        auto Search(StringView _Name) -> const char* { return nullptr; }

    public:
        struct FMemoryBlock
        {
            UInt32 ByteCursor  = 0;  //Point at the current byte.
            Byte*  Data;
        };

    private:
        //mutable FRWLock RWLock;
        Segment<FMemoryBlock, MaxMemoryBlocks> MemoryBlocks;
        UInt32 MemoryBlockCursor = 0; // Point at the current memory block.

    private:
        /*<< Helpers >>*/
    public://[TODO]:Remove (Testing)
        Tuple<Int32, UInt32> //[Number(<0 means invalid), NameLength]
        ParseNumber(const char* _Name, UInt32 _Length)
        {
            Int32 Number = -1; //Invalid Number
            
            //1. Find the First Digit.
            constexpr auto IsDigit = [](char _Char)->bool { return '0' <= _Char && _Char <= '9'; };
            UInt32 Digits = 0;
            for (const char* It = _Name + _Length - 1; It >= _Name && IsDigit(*It); --It)
            { ++Digits; }

            UInt32 FirstDigitCursor = _Length - Digits;
            
            if (/*Valid Digit Length*/      (Digits && Digits < _Length) &&
                /*Valid Naming Convention*/ (_Name[FirstDigitCursor - 1] == '_') &&
                /*Valid Digit Count*/       (Digits <= MaxNameDigitCount))
            {
                //Handle Special Cases
                if (/*Name_0 is Valid*/         Digits == 1 ||
                    /*Zero Prefix is InValid*/  _Name[FirstDigitCursor] != '0')
                {
                    Number = atoi(_Name + FirstDigitCursor);
                    if (Number < MaxNameNumber)
                    { return { Number , _Length - Digits - 1 }; }
                }
            }
            return { Number, _Length }; // No Valid Number
        }
    };

    void FNamePool::
    Register(const String& _Name)
    {
        auto [Number, NameLength] = ParseNumber(_Name.data(), _Name.size());
        if (Number < 0) { throw std::invalid_argument("Ref: Naming Convention"); }
        
        String Name = _Name.substr(0, NameLength);
        
        VE_ASSERT(MaxNameSlots == 256);
        union FNameHash
        {
            struct
            {
                struct
                {
                    int a : 2;
                    Byte Slot : 8;
                    //Bit(Slot, 8);
                };
                UInt32 Low32Bits;   //Determine the slot
            };

            UInt64 Value;
        };
        FNameHash Hash{ .Value = Hash::CityHash64(Name) };
        
    }

} } // namespace VE::Internal