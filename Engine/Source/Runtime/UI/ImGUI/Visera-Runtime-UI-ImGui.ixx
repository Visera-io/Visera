module;
#include <Visera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Runtime.UI.ImGui;

import Visera.Core.Signal;
import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{


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

} } // namespace VE::Runtime