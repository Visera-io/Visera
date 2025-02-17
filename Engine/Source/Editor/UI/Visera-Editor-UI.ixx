module;
#include <Visera.h>
export module Visera.Editor.UI;
import Visera.Runtime.RHI;

import :ImGui;

export namespace VE { namespace Editor
{
	using namespace VE::Runtime;

	class ViseraEditor;

	class UI
	{
		VE_MODULE_MANAGER_CLASS(UI);
	public://[TODO]:Remove  Testing Public
		static void Bootstrap()
		{
			ImGui = new FImGui();
		}

		static void Tick()
		{
			ImGui->BeginFrame();
		}

		static void RenderTick()
		{
			auto& CurrentFrame = RHI::GetCurrentFrame();
			VE_ASSERT(CurrentFrame.IsReady());
			ImGui->EndFrame(CurrentFrame.GetGraphicsCommandBuffer());
		}

		static void Terminate()
		{
			delete ImGui;
		}

	private:
		static inline FImGui* ImGui;
	};

} } // namespace VE::Editor