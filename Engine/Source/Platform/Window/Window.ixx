module;
#include <ViseraEngine>

#include <GLFW/glfw3.h>

export module Visera.Platform.Window;

import Visera.Core.Log;

export namespace VE
{
	#define SIT(Type) static inline Type

	class ViseraPlatform;

	class Window
	{
		friend class ViseraPlatform;
	public:
		static inline
		Bool
		Tick() { glfwPollEvents(); return !ShouldClose(); }

		static inline
		Bool
		ShouldClose() { return glfwWindowShouldClose(Handle); }

		static inline void
		SetWindowPosition(Int32 X, Int32 Y) { glfwSetWindowPos(Handle, X, Y); }
		
	private:
		SIT(String)		 Title		= VISERA_APP_NAME;
		SIT(UInt32)		 Width		= 1280;
		SIT(UInt32)		 Height		= 800;
		SIT(Bool)		 bMaximized	= False;

		SIT(GLFWwindow*) Handle		= nullptr;

	private:
		static inline void
		Bootstrap()
		{
			Log::Debug("Bootstrapping the System Window...");
			//Init GLFW
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE,	GLFW_TRUE);

			//Create Window
			Handle = glfwCreateWindow(Width, Height, Title.c_str(),NULL, NULL);
			if (!Handle) Log::Fatal("Failed to create GLFWwindow!");

			// Set Window Position
			const GLFWvidmode* VidMode = GetVideoMode(GetPrimaryMonitor());
			SetWindowPosition(
				(VidMode->width    -   Width ) >> 1,	// Mid
				(VidMode->height   -   Height) >> 1);	// Mid
			if (bMaximized) glfwMaximizeWindow(Handle);
		}

		static inline void
		Terminate()
		{
			Log::Debug("Terminating the System Window...");
			glfwDestroyWindow(Handle);
			glfwTerminate();
		}

		static inline
		GLFWmonitor*
		GetPrimaryMonitor() { return glfwGetPrimaryMonitor(); }

		static inline
		const GLFWvidmode*
		GetVideoMode(GLFWmonitor* Monitor) { return glfwGetVideoMode(Monitor); }
	};

} // namespace VE