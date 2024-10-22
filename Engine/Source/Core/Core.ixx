module;

export module Visera.Core;

export import Visera.Core.Base;
export import Visera.Core.Log;
export import Visera.Core.Math;

export namespace VE
{

	class ViseraEngine
	{
	public:
		static inline void
		Bootstrap()
		{
			Log::Debug("Bootstrapping Visera...");
		}

		static inline void
		Terminate()
		{
			Log::Debug("Terminating Visera...");
		}
	private:
		ViseraEngine() noexcept = default;
	};

} // namespace VE