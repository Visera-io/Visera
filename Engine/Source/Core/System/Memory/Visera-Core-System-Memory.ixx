module;
#include <Visera.h>
#if defined(VE_ON_X86_CPU)
#include <xmmintrin.h>
#elif defined(VE_ON_ARM_CPU)
#include <arm_acle.h>
#endif
export module Visera.Core.System.Memory;

import Visera.Core.Signal;

export namespace VE
{

    class Memory
    {
        VE_MODULE_MANAGER_CLASS(Memory);

        class FBuffer; //RAII
        VE_API Allocate(UInt64 _Size, UInt32 _Alignment = 0) throw (SRuntimeError) { return CreateSharedPtr<FBuffer>(_Size, _Alignment); }
        VE_API AllocateNow(UInt64 _Size, UInt32 _Alignment = 0, Int32 _Value = 0) { auto Buffer = CreateSharedPtr<FBuffer>(_Size, _Alignment); Memset(Buffer->GetData(), _Value, Buffer->GetSize()); return Buffer; }

        VE_API Malloc(UInt64 _Size, UInt32 _Alignment) throw (SRuntimeError) -> void*;
        VE_API Memset(void* _Memory, Int32 _Value, UInt64 _Size) -> void;
        VE_API MallocNow(UInt64 _Size, UInt32 _Alignment, Int32 _Value = 0) -> void* { void* AllocatedMemory = Malloc(_Size, _Alignment); Memset(AllocatedMemory, _Value, _Size); return AllocatedMemory; }
        VE_API Realloc(void* _Memory, UInt64 _NewSize, UInt32 _NewAlignment) throw (SRuntimeError) -> void*;
        VE_API Free(void* _Memory, UInt32 _Alignment) -> void;

        VE_API Prefetch(const void* _Memory, UInt32 _Offset = 0) -> void;
        
        VE_API IsValidAllocation(UInt64 _Size, UInt32 _Alignment)   ->Bool;
        VE_API IsZero(const void* _Memory, UInt64 _Size)            ->Bool;

        /*Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two.*/
        template <Alignable T> static constexpr
        T Align(T _Target, UInt64 _Alignment) { VE_ASSERT(IsPowerOfTwo(_Alignment)); return (T)(((UInt64)_Target + _Alignment - 1) & ~(_Alignment - 1)); };

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
            FBuffer(UInt64 _Size, UInt32 _Alignment = 0);
            ~FBuffer();

        private:
            void*  Data         = nullptr;
            UInt64 Size         = 0; // Note that system will allocate at least 1Byte memory.
            UInt32 Alignment    = 0; // No Alignment
        };

        /*C-Style Unsafe Allocation*/

        
    };

    Memory::FBuffer::FBuffer(UInt64 _Size, UInt32 _Alignment/* = 0*/)
        :Size{_Size},
         Alignment{_Alignment},
         Data{ Memory::Malloc(_Size, _Alignment) }
    { 
        
    }

    Memory::FBuffer::
    ~FBuffer()
    { 
        Memory::Free(Data, Alignment);
    }

    void* Memory::
    Malloc(UInt64 _Size, UInt32 _Alignment)
    throw (SRuntimeError)
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

    void Memory::Memset(void* _Memory, Int32 _Value, UInt64 _Size)
    {
        memset(_Memory, _Value, _Size);
    }

    void* Memory::
    Realloc(void* _Memory, UInt64 _NewSize, UInt32 _NewAlignment)
    throw (SRuntimeError)
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

    void Memory::
    Free(void* _Memory, UInt32 _Alignment)
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

    void Memory::
    Prefetch(const void* _Memory, UInt32 _Offset/* = 0*/)
    {
#if defined(VE_ON_X86_CPU)
        _mm_prefetch((const char*)_Memory + _Offset, _MM_HINT_T0);
#elif defined(VE_ON_ARM_CPU)
        __pld((const char*)_Memory + _Offset); //[FIXME]: Not be tested!
#endif
    }

    Bool Memory::
    IsValidAllocation(UInt64 _Size, UInt32 _Alignment)
    { return _Size && (!_Alignment || (IsPowerOfTwo(_Alignment) && (_Size % _Alignment == 0))); }

    Bool Memory::
    IsZero(const void* _Memory, UInt64 _Size)
    { 
        Byte* Start = (Byte*)_Memory;
        Byte* End = Start + _Size;
        while (Start < End)
        {  if ((*Start++) != 0) return False; }
        return True;
    }

} // namespace VE