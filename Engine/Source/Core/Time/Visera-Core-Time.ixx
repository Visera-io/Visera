module;
#include <Visera.h>

export module Visera.Core.Time;
export import :Clock;

VISERA_PUBLIC_MODULE

class Time
{
public:
	VE_INTERFACE Now() { return Clock.GetTotalTime(); }

private:
	static inline SystemClock Clock;
};

VISERA_MODULE_END