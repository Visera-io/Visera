module;
#include <Visera.h>
export module Visera.Internal.NamePool;
import :Common;
import :NameTokenTable;
import :NameEntryTable;

import Visera.Core.Math.Hash;
import Visera.Core.Signal;
import Visera.Core.Log;
import Visera.Core.System;

export namespace VE { namespace Internal
{

    class FNamePool
    {
        enum
        {
            MaxNameDigitCount   = 10,  //Sub-Effects:...
            MaxNameNumber       = INT32_MAX,
        };
    public:
        auto Register(StringView _Name) throw (SRuntimeError) -> ResultPackage<UInt32, UInt32>; //[Handle_, Number_]
        //auto Search(StringView _Name) const -> StringView;

    private:
        FNameTokenTable NameTokenTable;
        FNameEntryTable NameEntryTable;

        //[Number(<0 means invalid), NameLength]
        auto ParseName(const char* _Name, UInt32 _Length) const -> ResultPackage<Int32, UInt32>;
    };

    ResultPackage<UInt32, UInt32> FNamePool::
    Register(StringView _Name)
    {
        Log::Debug("Registering a new FName:{}", _Name);

        auto [Number, NameLength] = ParseName(_Name.data(), _Name.size());
        if (Number < 0) { throw SRuntimeError("Bad Name! -- Naming Convention:([#Name][_#Number]?)."); }

        StringView PureName{ _Name.data(), NameLength};
        FNameHash  NameHash{ PureName };

        FNameTokenHandle NameTokenHandle = NameTokenTable.Insert(NameEntryTable, NameHash);

        //auto BookMark = NameDictionary.Insert(NewNameEntry.Header, PureName);

        /*auto& TargetShard = Shards[NameHash.GetShardIndex()];
        UInt32 HashAndID = 0;
        TargetShard.RWLock.StartWriting();
        {
            if (TargetShard.ShouldGrow())
            { GrowAndRehash(&TargetShard, NameDictionary); }

            auto& Slot = TargetShard.ProbeSlot(NameHash.GetUnmaskedTokenIndex());
            HashAndID  = NameHash.GetSlotProbeHash() | FNameEntryID(BookMark).GetID();
            TargetShard.ClaimSlot(&Slot, HashAndID);
        }
        TargetShard.RWLock.StopWriting();*/
        
        VE_ASSERT(False); //WIP
        return {};
        //return { HashAndID, Number }; // Numer == 0 means NO Number
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