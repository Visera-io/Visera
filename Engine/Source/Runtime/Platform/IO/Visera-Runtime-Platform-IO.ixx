module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform.IO;
import :Common;
import :Keyboard;
import :Mouse;

import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Math.Basic;

import Visera.Runtime.Platform.Window;

export namespace VE
{
	
	class IO
	{
		VE_MODULE_MANAGER_CLASS(IO);
	public:
		using EAction      = EAction;
		using EKey         = EKey;
		using EMouseButton = EMouseButton;

        using FKeyboardKeyEventCreateInfo = Keyboard::FKeyEventCreateInfo;

        static inline void
		ProcessEvents() { Keyboard::ProcessEvents(); }

		static inline auto
		GetMouseCursor() -> const Mouse::FCursor& { return Mouse::Cursor; }
		static inline auto
		GetMouseScroll() -> const Mouse::FScroll& { return Mouse::Scroll; }

		static inline void
		RegisterKeyboardKeyEvent(const FKeyboardKeyEventCreateInfo& _CreateInfo) { Keyboard::RegisterKeyEvent(_CreateInfo); };
		static inline void
		DeleteKeyboardKeyEvent(const FName& _Name) { Keyboard::DeleteKeyEvent(_Name); };

		static inline void
		Bootstrap();
		static inline void
		Terminate() { }
	};

	void IO::
	Bootstrap()
	{ 
		if (Window::GetHandle() == nullptr)
		{ throw SRuntimeError("Failed to bootstrap IO - you must bootstrap Window first!"); }

		glfwSetKeyCallback      (Window::GetHandle(), Keyboard::KeyCallback);

		glfwSetCursorPosCallback(Window::GetHandle(), Mouse::CursorCallback);
		glfwSetScrollCallback   (Window::GetHandle(), Mouse::ScrollCallback);
	}

} // namespace VE