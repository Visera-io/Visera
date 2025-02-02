module;
#include <Visera.h>
export module Visera.Editor;
export import Visera.Editor.UI;

namespace VE { class Visera; }

export namespace VE { namespace Editor {

	class ViseraEditor
	{
		VE_MODULE_MANAGER_CLASS(ViseraEditor);
		friend class Visera;

		static void Bootstrap() {};
		static void Tick() {};
		static void Terminate() {};
	};

} } // namespace VE::Editor