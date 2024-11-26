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
		virtual void Bootstrap() = 0;
		virtual void Terminate() = 0;
		virtual void Tick() = 0;
		virtual void RenderTick() = 0;
		
		void inline
		Exit(const AppExitSignal& Message = AppExitSignal("Visera App Exited Successfully.")) const throw(AppExitSignal) { throw Message; }

		ViseraApp()	 noexcept = default;
		~ViseraApp() noexcept = default;
	};

} // namespace VE
