module;
#include <Visera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor.UI:ImGui;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
class UI;

class FImGui
{
	friend class UI;
public:

private:
	using CreateInfo = ImGui_ImplVulkan_InitInfo;

	inline void
	Bootstrap(const CreateInfo& _CreateInfo)
	{
		
	}
	inline void
	Terminate()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
};

} } // namespace VE::Runtime