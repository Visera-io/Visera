module;
#include <Visera.h>

export module Visera.Runtime.Context;

import Visera.Core.Log;

VISERA_PUBLIC_MODULE
class ViseraRuntime;

class RuntimeContext
{
	friend class ViseraRuntime;
public:
	static inline struct
	{
		enum State: EnumBit
		{
			Running  = 0,
			Stopping = 1 << 0, 
		};
		EnumMask States = Running;
		Bool ShouldStop() const { return States & Stopping; }

		void Stop(const std::source_location& Location = std::source_location::current())
		{
			States |= Stopping;
			std::stringstream SS;
			SS  << "[Stop Location]"
				<< "\n- File: "		<< Location.file_name()
				<< "\n- Line: "		<< Location.line()
				<< "\n- Function: "	<< Location.function_name();
			Log::Warn("Stopping Visera Main Loop...\n{}", SS.str());
		}
	}MainLoop;

	static inline struct
	{
		Bool IsOffScreenRendering() const { return False; }
	}Render;

private:
	static inline void
	Bootstrap()
	{

	}

	static inline void
	Terminate()
	{

	}
};

VISERA_MODULE_END