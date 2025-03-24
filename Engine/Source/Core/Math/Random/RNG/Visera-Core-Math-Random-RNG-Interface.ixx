module;
#include <Visera.h>
export module Visera.Core.Math.Random.RNG:Interface;

export namespace VE
{

    class IRNG
    {
        static inline std::random_device RandomDevice;
    public:
        virtual auto Random() -> Float = 0; //[0,1)

        IRNG() = default;
        virtual ~IRNG() = default;

    protected:
        UInt32 Seed = RandomDevice();
        std::uniform_real_distribution<Float>
        UniformFloatDistribution{0, 1 - std::numeric_limits<float>::epsilon()};
    };

}; // namespace VE