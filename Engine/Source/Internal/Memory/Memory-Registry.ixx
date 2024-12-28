module;
#include <Visera.h>

export module Visera.Internal.Memory:Registry;

import Visera.Internal.Pattern;

export namespace VE
{

    class MemoryRegistry :
        public Singleton<MemoryRegistry>
    {
        friend class Singleton<MemoryRegistry>;
    public:
        String MemFormatVector;
        String MemFormatMatrix;

        MemoryRegistry()
        {
            MemFormatVector.resize(/*Vector4F*/(4 * 32) + /*(X, Y, Z, W)*/(1 + 3*2 + 1));
            MemFormatMatrix.resize(/*Matrix4x4F*/(16 * 32) +
							       /*{A0, A1, A2, A3,
							          B0, B1, B2, B3,
							          C0, C1, C2, C3,
							          D0, D1, D2, D3}*/ 9 * 4 - 1);
        }
    };
    
} // namespace VE