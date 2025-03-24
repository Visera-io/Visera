module;
#include <Visera.h>
export module Visera.Core.Math.Random.RNG:MT19937;
import :Interface;

export namespace VE
{

    class FMT19937 : public IRNG
    {
    public:
        virtual auto Random() -> Float override
        { return UniformFloatDistribution(RandomEngine); }

        FMT19937() = default;
    private:
        std::mt19937 RandomEngine{ Seed };
    };

}; // namespace VE