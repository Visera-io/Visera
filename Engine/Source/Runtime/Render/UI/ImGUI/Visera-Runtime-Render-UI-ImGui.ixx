module;
#include <Visera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Runtime.Render.UI.ImGui;

import Visera.Core.Signal;
import Visera.Runtime.Render.RHI;

VISERA_PUBLIC_MODULE

class UI;
#define SI static inline

class ImGui
{
	friend class UI;
public:

private:

	
private:
	using CreateInfo = ImGui_ImplVulkan_InitInfo;

	static inline void
	Bootstrap(const CreateInfo& _CreateInfo)
	{
		
	}
	static inline void
	Terminate()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
};

VISERA_MODULE_END