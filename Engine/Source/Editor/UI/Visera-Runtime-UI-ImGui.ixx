module;
#include <Visera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor.UI:ImGui;

import Visera.Core.Signal;

export namespace VE { namespace Editor
{

	class UI;

	class FImGui
	{
		friend class UI;
	public:

	private:
		using CreateInfo = ImGui_ImplVulkan_InitInfo;

		FImGui(const CreateInfo& _CreateInfo)
		{
		
		}
	
		~FImGui()
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}
	};

} } // namespace VE::Editor