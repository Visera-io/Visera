module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform.Window;
#define VE_MODULE_NAME "Window"

import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE
{
	class Window
	{
		VE_MODULE_MANAGER_CLASS(Window);
	public:
		union FExtent
		{
			struct { Int32 Width, Height; };
			struct { Int32 X	, Y		; };
			Bool IsValid() const { return Width >= 0 && Height >= 0; }
		};
		struct FContentScale{ Float X, Y; };

		static inline Bool
		ShouldClose() { return glfwWindowShouldClose(Handle); }
		static inline void
		PollEvents() { glfwPollEvents(); }

		static inline GLFWwindow*
		GetHandle() { return Handle; }
		static inline StringView
		GetTitle() { return Title; }
		static inline FExtent
		GetExtent() { return CurrentExtent; }
		static inline const FContentScale&
		GetContentScale() { return ContentScale; }

		static inline void
		QueryContentScale(Float* _ScaleX, Float* _ScaleY) { glfwGetWindowContentScale(Handle, _ScaleX, _ScaleY); }
		static inline FExtent
		QueryFrameBufferExtent() { FExtent FrameBufferFExtent; glfwGetFramebufferSize(Handle, &FrameBufferFExtent.Width, &FrameBufferFExtent.Height); return FrameBufferFExtent; }
		static inline void
		QueryFrameBufferExtent(FExtent* Result) { glfwGetFramebufferSize(Handle, &Result->Width, &Result->Height); }
	
	private:
		static inline void
		SetSize(Int32 _Width, Int32 _Height) { glfwSetWindowSize(Handle, _Width, _Height); }
		static inline void
		SetPosition(Int32 _X, Int32 _Y) { glfwSetWindowPos(Handle, _X, _Y); }
		static inline GLFWmonitor*
		GetPrimaryMonitor() { return glfwGetPrimaryMonitor(); }
		static inline const GLFWvidmode*
		GetVideoMode(GLFWmonitor* _Monitor) { return glfwGetVideoMode(_Monitor); }

	private:
		static inline String	Title				= VISERA_APP_NAME;
		static inline FExtent	CurrentExtent		{{.Width = 1600, .Height = 900}};
		static inline Bool		bMaximized			= False;
		static inline FContentScale ContentScale;

		static inline GLFWwindow*  Handle		= nullptr;

	private:
		VE_API Bootstrap() -> void
		{
			//Init GLFW
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE,	GLFW_TRUE);

			//Create Window
			Handle = glfwCreateWindow(
				CurrentExtent.Width, CurrentExtent.Height,
				Title.c_str(),
				NULL,
				NULL);
			if (!Handle)
			{ throw SRuntimeError("Failed to create GLFWwindow!"); }
			
			if (!glfwVulkanSupported())
			{ throw SRuntimeError("GLFW: Vulkan Not Supported\n"); }

			// Set Window Position
			const GLFWvidmode* VidMode = GetVideoMode(GetPrimaryMonitor());
#if (VE_IS_APPLE_SYSTEM)
			SetPosition(400, 200);
#else
			SetPosition(
				(VidMode->width    -   CurrentExtent.Width ) >> 1,	// Mid
				(VidMode->height   -   CurrentExtent.Height) >> 1);	// Mid
#endif

			QueryContentScale(&ContentScale.X, &ContentScale.Y);

			if (bMaximized) { glfwMaximizeWindow(Handle); }
			else
			{
				if (ContentScale.X != 1.0 || ContentScale.Y != 1.0)
				{
					SetSize(CurrentExtent.Width / ContentScale.X,
					        CurrentExtent.Height / ContentScale.Y);
				}
			}

			VE_LOG_DEBUG("Created a new window (title:{}, extent:[{},{}], scales:[{},{}])",
				Title, CurrentExtent.Width, CurrentExtent.Height, ContentScale.X, ContentScale.Y);
		}
		
		VE_API Terminate() -> void
		{
			glfwDestroyWindow(Handle);
			glfwTerminate();
		}
	};

} // namespace VE