module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform:Window;

import Visera.Core.Log;

VISERA_PUBLIC_MODULE
class Platform;

class Window : public Singleton<Window>
{
	friend class Platform;
	friend class Singleton<Window>;
public:
	union Extent
	{
		struct { Int32 Width, Height; };
		struct { Int32 X	, Y		; };
		Bool IsValid() const { return Width >= 0 && Height >= 0; }
	};

public:
	GLFWwindow*
	GetHandle() const { return Handle; }
	StringView
	GetTitle() const { return Title; }
	Bool
	ShouldClose() const { return glfwWindowShouldClose(Handle); }
	void
	PollEvents() const { glfwPollEvents(); }
	Extent
	QueryFrameBufferExtent() const { Extent FrameBufferExtent; glfwGetFramebufferSize(Handle, &FrameBufferExtent.Width, &FrameBufferExtent.Height); return FrameBufferExtent; }
	void
	QueryFrameBufferExtent(Extent* Result) const { glfwGetFramebufferSize(Handle, &Result->Width, &Result->Height); }
private:
	void
	SetPosition(Int32 X, Int32 Y) const { glfwSetWindowPos(Handle, X, Y); }
	GLFWmonitor*
	GetPrimaryMonitor() { return glfwGetPrimaryMonitor(); }
	const GLFWvidmode*
	GetVideoMode(GLFWmonitor* Monitor) { return glfwGetVideoMode(Monitor); }


private:
	String		 Title				= VISERA_APP_NAME;
	Extent		 CurrentExtent		{{.Width = 1280, .Height = 800}};
	Bool		 bMaximized			= False;

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
		Handle = glfwCreateWindow(CurrentExtent.Width, CurrentExtent.Height, Title.c_str(), NULL, NULL);
		if (!Handle) Log::Fatal("Failed to create GLFWwindow!");

		// Set Window Position
		const GLFWvidmode* VidMode = GetVideoMode(GetPrimaryMonitor());
		SetPosition(
			(VidMode->width    -   CurrentExtent.Width ) >> 1,	// Mid
			(VidMode->height   -   CurrentExtent.Height) >> 1);	// Mid
		if (bMaximized) glfwMaximizeWindow(Handle);
	}

	~Window()
	{
		Log::Debug("Terminating the System Window...");
		glfwDestroyWindow(Handle);
		glfwTerminate();
	}
};

VISERA_MODULE_END