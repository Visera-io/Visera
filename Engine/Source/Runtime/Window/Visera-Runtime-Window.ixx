module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Window;
import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class ViseraRuntime;

	class Window
	{
		VE_MODULE_MANAGER_CLASS(Window);
		friend class ViseraRuntime;
	public:
		union Extent
		{
			struct { Int32 Width, Height; };
			struct { Int32 X	, Y		; };
			Bool IsValid() const { return Width >= 0 && Height >= 0; }
		};

	public:
		static inline GLFWwindow*
		GetHandle() { return Handle; }
		static inline StringView
		GetTitle() { return Title; }
		static inline Bool
		ShouldClose() { return glfwWindowShouldClose(Handle); }
		static inline void
		PollEvents() { glfwPollEvents(); }
		static inline Extent
		QueryFrameBufferExtent() { Extent FrameBufferExtent; glfwGetFramebufferSize(Handle, &FrameBufferExtent.Width, &FrameBufferExtent.Height); return FrameBufferExtent; }
		static inline void
		QueryFrameBufferExtent(Extent* Result) { glfwGetFramebufferSize(Handle, &Result->Width, &Result->Height); }
	
	private:
		static inline void
		SetPosition(Int32 X, Int32 Y) { glfwSetWindowPos(Handle, X, Y); }
		static inline GLFWmonitor*
		GetPrimaryMonitor() { return glfwGetPrimaryMonitor(); }
		static inline const GLFWvidmode*
		GetVideoMode(GLFWmonitor* Monitor) { return glfwGetVideoMode(Monitor); }

	private:
		static inline String	Title				= VISERA_APP_NAME;
		static inline Extent	CurrentExtent		{{.Width = 1280, .Height = 800}};
		static inline Bool		bMaximized			= False;

		static inline GLFWwindow*  Handle		= nullptr;

	private:
		VE_API Bootstrap() -> void
		{
			//Init GLFW
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE,	GLFW_TRUE);

			//Create Window
			Handle = glfwCreateWindow(CurrentExtent.Width, CurrentExtent.Height, Title.c_str(), NULL, NULL);
			if (!Handle) throw SRuntimeError("Failed to create GLFWwindow!");

			// Set Window Position
			const GLFWvidmode* VidMode = GetVideoMode(GetPrimaryMonitor());
			SetPosition(
				(VidMode->width    -   CurrentExtent.Width ) >> 1,	// Mid
				(VidMode->height   -   CurrentExtent.Height) >> 1);	// Mid
			if (bMaximized) glfwMaximizeWindow(Handle);
		}
		VE_API Tick() -> void
		{
			glfwPollEvents();
		}
		VE_API Terminate() -> void
		{
			glfwDestroyWindow(Handle);
			glfwTerminate();
		}
	};

} } // namespace VE::Runtime