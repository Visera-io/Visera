module;
#include <Visera.h>
#include <cstdint>
export module Visera.Core.System.Memory;

import Visera.Core.Signal;

export namespace VE
{

    class Memory
    {  
    public:
        class FBuffer; //RAII

        static inline
        SharedPtr<Memory::FBuffer>
        Allocate(UInt64 _Size, UInt32 _Alignment = 0) throw (SRuntimeError) { return CreateSharedPtr<FBuffer>(_Size, _Alignment); }

    public:  /*C-Style Unsafe Allocation*/

        static inline
        void*
        Malloc(UInt64 _Size, UInt32 _Alignment = 0) throw (SRuntimeError)
        { 
            void* AllocatedMemory = nullptr;
            if (_Alignment)
            {
                VE_ASSERT(IsPowerOfTwo(_Alignment));
#if defined(VE_IS_WINDOWS_SYSTEM)
                AllocatedMemory = _aligned_malloc(_Size, _Alignment);
#else
                AllocatedMemory = std::aligned_malloc(_Size, _Alignment);
#endif
            }
            else AllocatedMemory = std::malloc(_Size);
            
            if (!AllocatedMemory)
            { throw SRuntimeError(Text("Failed to allocate memory! (size: {}, alignment: {})", _Size, _Alignment)); }
            
            return AllocatedMemory;
        }

        static inline void*
        Realloc(void* _Memory, UInt64 _NewSize, UInt32 _NewAlignment = 0)
        {
            void* ReallocatedMemory = nullptr;
            if (_NewAlignment)
            {
                VE_ASSERT(IsPowerOfTwo(_NewAlignment));
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

        static inline void
        Free(void* _Memory, UInt32 _Alignment = 0)
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

        static inline
        Bool
        IsZero(const void* _Memory, UInt64 _Size)
        { 
            Byte* Start = (Byte*)_Memory;
            Byte* End = Start + _Size;
            while (Start < End)
            {  if ((*Start++) != 0) return False; }
            return True;
        }

    public:

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
            { VE_ASSERT(Size > 0); Data = Memory::Malloc(Size, Alignment); }
            ~FBuffer() { Memory::Free(Data, Alignment); }

        private:
            void*  Data         = nullptr;
            UInt64 Size         = 0;
            UInt32 Alignment    = 0;
        };
    };

} // namespace VE