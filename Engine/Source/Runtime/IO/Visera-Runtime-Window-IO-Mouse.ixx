module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.IO:Mouse;
import :Common;

import Visera.Core.Type;
import Visera.Core.Math.Basic;

import Visera.Runtime.Window;

export namespace VE
{
	
	class Mouse
	{
		VE_MODULE_MANAGER_CLASS(Mouse);
	public:
		using FButtonEvent = std::function<void()>;
		struct FButtonEventCreateInfo
		{
			FName		 Name;
			EMouseButton Button;
			EAction		 Action;
			FButtonEvent Event;
		};

		using FScrollEvent = std::function<void(double _X, double _Y)>;
		struct FScrollEventCreateInfo
		{
			FName		 Name;
			FScrollEvent Event;
		};

		struct FCursor
		{
			Vector2F Position;
			Vector2F Offset;
		};
		static inline FCursor Cursor;

		struct FScroll
		{
			Vector2F Offset;
		};
		static inline FScroll Scroll;

		static inline void
		ScrollCallback(GLFWwindow* window, double _OffsetX, double _OffsetY)
		{
			Scroll.Offset.x() = _OffsetX;
			Scroll.Offset.y() = _OffsetY;
		}

		static inline void
		CursorCallback(GLFWwindow* _Window, double _CurrentX, double _CurrentY)
		{
			static const auto ContentScale = Window::GetContentScale();
			_CurrentX *= ContentScale.X;
			_CurrentY *= ContentScale.Y;

			Cursor.Offset.x()   = _CurrentX - Cursor.Position.x();
			Cursor.Offset.y()   = _CurrentY - Cursor.Position.y();
			Cursor.Position.x() = _CurrentX;
			Cursor.Position.y() = _CurrentY;
		}
	};

} // namespace VE