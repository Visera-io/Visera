module;
#include <Visera.h>
export module Visera.Internal.NamePool;
import :Common;
import :NameTokenTable;
import :NameEntryTable;

import Visera.Core.Math.Hash;
import Visera.Core.Signal;

export namespace VE { namespace Internal
{
    enum class EName : UInt32
	{
		None = 0, //MUST be registered at first for assuring FNameEntryID == 0 && Number == 0

		MaxReservedName,
	};

    class FNamePool
    {
        enum
        {
            MaxNameDigitCount   = 10,  //Sub-Effects:...
            MaxNameNumber       = INT32_MAX,
        };
    public:
        auto Register(StringView _Name) throw (SRuntimeError) -> ResultPackage<UInt32, UInt32>; //[Handle_, Number_]
        auto Search(UInt32 _NameHandle /*NameEntryHandle*/) const->StringView;
        auto Search(EName _PreservedName) const -> StringView;

        FNamePool();
        ~FNamePool() = default;
    private:
        FNameEntryTable NameEntryTable;
        FNameTokenTable NameTokenTable{ &NameEntryTable };

        //[Number(<0 means invalid), NameLength]
        auto ParseName(const char* _Name, UInt32 _Length) const -> ResultPackage<Int32, UInt32>;
    };

    FNamePool::
    FNamePool()
    {
        // Pre-Register ENames (Do NOT use String Literal here -- Read-Only Segment Fault!)
        /*None*/ auto [Handle_, Number_] = Register(String("none")); VE_ASSERT(Handle_ == 0);
    }

    ResultPackage<UInt32, UInt32> FNamePool::
    Register(StringView _Name)
    {
        auto [Number, NameLength] = ParseName(_Name.data(), _Name.size());
        if (Number < 0) { throw SRuntimeError("Bad Name! -- Naming Convention:([#Name][_#Number]?)."); }

        StringView PureName{ _Name.data(), NameLength};
        FNameHash  NameHash{ PureName };
        
        FNameEntryHandle NameEntryHandle = NameTokenTable.Insert(PureName, NameHash);

        return { NameEntryHandle.Value, Number };
    }

    StringView FNamePool::
    Search(UInt32 _NameHandle /*NameEntryHandle*/) const
    {
        FNameEntryHandle NameEntryHandle;
        NameEntryHandle.Value = _NameHandle;
        VE_ASSERT(NameEntryHandle.GetSectionIndex()  < FNameEntryTable::MaxSections &&
                  NameEntryHandle.GetSectionOffset() < FNameEntryTable::MaxSectionOffset);
        auto& R = NameEntryTable.LookUp(NameEntryHandle);

        return NameEntryTable.LookUp(NameEntryHandle).GetANSIName();
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

    StringView FNamePool::
    Search(EName _PreservedName) const
    {
        switch (_PreservedName)
        {
        case EName::None:
            return "";
        default:
            throw SRuntimeError("Unexpected EName!");
        }
    }

} } // namespace VE::Internal