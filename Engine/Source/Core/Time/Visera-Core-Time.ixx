module;
#include <Visera.h>

export module Visera.Core.Time;
export import :Clock;

export namespace VE
{


class Time
{
public:
	VE_INTERFACE Now() { return Clock.GetTotalTime(); }

private:
	static inline SystemClock Clock;
};

} // namespace VE