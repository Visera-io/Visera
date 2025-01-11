module;
#include <Visera.h>
export module Visera.Internal.Memory;
import :NamePool;

export namespace VE { namespace Internal
{

    class Memory
    {
    public:
        static inline auto
        GetNamePool() -> FNamePool& { return NamePool; }

    private:
        static inline FNamePool NamePool;
    };

} } // namespace VE::Internal