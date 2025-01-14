module;
#include <Visera.h>
export module Visera.Core.System.Memory;

import Visera.Core.Signal;

export namespace VE
{

    class Memory
    {
        VE_MODULE_MANAGER_CLASS(Memory);
    public:
        class FBuffer; //RAII

        VE_API Allocate(UInt64 _Size, UInt32 _Alignment = 0) throw (SRuntimeError) { return CreateSharedPtr<FBuffer>(_Size, _Alignment); }

        class FBuffer final
        {
        public:
            auto GetData()            -> void*  { return Data;}
            auto GetSize()      const -> UInt64 { return Size; }
            auto GetAlignment() const -> UInt32 { return Alignment; }

            Bool IsAligned()    const { return Alignment != 0; }
            Bool IsZero() const { return Memory::IsZero(Data, Size); }
            void Resize(UInt64 _NewSize, UInt32 _NewAlignment = 0) { if (!_NewAlignment) _NewAlignment = Alignment; Data = Memory::Realloc(Data, _NewSize, _NewAlignment); Size = _NewSize; Alignment = _NewAlignment; }

            FBuffer() = delete;
            FBuffer(UInt64 _Size, UInt32 _Alignment = 0)
                :Size{_Size}, Alignment{_Alignment}
            { VE_ASSERT(Size); Data = Memory::Malloc(Size, Alignment); }
            ~FBuffer() { Memory::Free(Data, Alignment); }

        private:
            void*  Data         = nullptr;
            UInt64 Size         = 0; // Note that system will allocate at least 1Byte memory.
            UInt32 Alignment    = 0; // No Alignment
        };

    public:  /*C-Style Unsafe Allocation*/

        VE_API Malloc(UInt64 _Size, UInt32 _Alignment)
        throw (SRuntimeError) -> void*
        { 
            VE_ASSERT(IsValidAllocation(_Size, _Alignment));

            void* AllocatedMemory = nullptr;
            if (_Alignment)
            {
#if defined(VE_IS_WINDOWS_SYSTEM)
                AllocatedMemory = _aligned_malloc(_Size, _Alignment);
#else
                AllocatedMemory = std::aligned_alloc(_Size, _Alignment);
#endif
            }
            else AllocatedMemory = std::malloc(_Size);
            
            if (!AllocatedMemory)
            { throw SRuntimeError(Text("Failed to allocate memory! (size: {}, alignment: {})", _Size, _Alignment)); }
            
            return AllocatedMemory;
        }

        VE_API Realloc(void* _Memory, UInt64 _NewSize, UInt32 _NewAlignment)
        throw (SRuntimeError) -> void*
        {
            VE_ASSERT(IsValidAllocation(_NewSize, _NewAlignment));

            void* ReallocatedMemory = nullptr;
            if (_NewAlignment)
            {
#if defined(VE_IS_WINDOWS_SYSTEM)
                ReallocatedMemory = _aligned_realloc(_Memory, _NewSize, _NewAlignment);
#else
                ReallocatedMemory = std::aligned_realloc(_Memory, _NewSize, _NewAlignment);
#endif 
            }
            else ReallocatedMemory = std::realloc(_Memory, _NewSize);

            if (!ReallocatedMemory)
            { throw SRuntimeError(Text("Failed to re-allocate memory! (size: {}, alignment: {})", _NewSize, _NewAlignment)); }  
            
            return ReallocatedMemory;
        }

        VE_API Free(void* _Memory, UInt32 _Alignment)
        { 
            if (_Alignment)
            {
                VE_ASSERT(IsPowerOfTwo(_Alignment));
#if defined(VE_IS_WINDOWS_SYSTEM)
                _aligned_free(_Memory);
#else
                std::aligned_free(_Memory);
#endif
            }
            else std::free(_Memory);
        }

        VE_API IsValidAllocation(UInt64 _Size, UInt32 _Alignment) -> Bool
        { return _Size && (!_Alignment || (IsPowerOfTwo(_Alignment) && (_Size % _Alignment == 0))); }

        VE_API IsZero(const void* _Memory, UInt64 _Size) -> Bool
        { 
            Byte* Start = (Byte*)_Memory;
            Byte* End = Start + _Size;
            while (Start < End)
            {  if ((*Start++) != 0) return False; }
            return True;
        }
    };

} // namespace VE