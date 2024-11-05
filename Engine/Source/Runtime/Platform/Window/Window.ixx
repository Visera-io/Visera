module;
#include <ViseraEngine>

#include <GLFW/glfw3.h>

export module Visera.Runtime.Platform.Window;

import Visera.Core.Log;

export namespace VE
{

	class PlatformRuntime;

	class Window
	{
		friend class PlatformRuntime;
	public:
		GLFWwindow*
		GetHandle() const { return Handle; }
		
		Bool
		ShouldClose() const { return glfwWindowShouldClose(Handle); }

		void
		PollEvents() const { glfwPollEvents(); }

	private:
		void
		SetPosition(Int32 X, Int32 Y) const { glfwSetWindowPos(Handle, X, Y); }

		GLFWmonitor*
		GetPrimaryMonitor() { return glfwGetPrimaryMonitor(); }

		const GLFWvidmode*
		GetVideoMode(GLFWmonitor* Monitor) { return glfwGetVideoMode(Monitor); }


	private:
		String		 Title		= VISERA_APP_NAME;
		UInt32		 Width		= 1280;
		UInt32		 Height		= 800;
		Bool		 bMaximized	= False;

		GLFWwindow*  Handle		= nullptr;

	private:
		Window()
		{
			Log::Debug("Bootstrapping the System Window...");
			//Init GLFW
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE,	GLFW_TRUE);

			//Create Window
			Handle = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL);
			if (!Handle) Log::Fatal("Failed to create GLFWwindow!");

			// Set Window Position
			const GLFWvidmode* VidMode = GetVideoMode(GetPrimaryMonitor());
			SetPosition(
				(VidMode->width    -   Width ) >> 1,	// Mid
				(VidMode->height   -   Height) >> 1);	// Mid
			if (bMaximized) glfwMaximizeWindow(Handle);
		}

		~Window()
		{
			Log::Debug("Terminating the System Window...");
			glfwDestroyWindow(Handle);
			glfwTerminate();
		}
	};

} // namespace VE