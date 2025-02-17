module;
#include <Visera.h>
export module Visera.Editor;
export import Visera.Editor.UI;

namespace VE { class Visera; }

export namespace VE { namespace Editor 
{

	class ViseraEditor
	{
		VE_MODULE_MANAGER_CLASS(ViseraEditor);
		friend class Visera;

		VE_API Bootstrap()
		{
			UI::Bootstrap();
		};
		VE_API Tick()
		{
			UI::Tick();
		};
		VE_API RenderTick()
		{
			UI::RenderTick();
		};
		VE_API Terminate()
		{
			UI::Terminate();
		};
	};

} } // namespace VE::Editor