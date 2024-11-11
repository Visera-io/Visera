module;
#include <Visera>

export module Visera.App;
export import Visera.App.Base;
export import Visera.App.IO;
export import Visera.App.Log;
export import Visera.App.Math;
export import Visera.App.Render;

import Visera.Engine.Core.Signal;

export namespace VE
{  

	class ViseraApp
	{
	public:
		virtual void
		Tick() { Exit(); }
		virtual void
		RenderTick() {}
		void inline
		Exit(RawString Message = "Visera App Exited Successfully.") const throw(AppExitSignal) { throw AppExitSignal(Message); }

		ViseraApp()			 = default;
		virtual ~ViseraApp() = default;
	};

} // namespace VE
