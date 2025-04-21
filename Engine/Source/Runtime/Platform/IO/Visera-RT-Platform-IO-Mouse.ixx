module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform.IO:Mouse;
#define VE_MODULE_NAME "IO:Mouse"
import :Common;
import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.Math.Basic;
import Visera.Core.OS.Concurrency;

import Visera.Runtime.Platform.Window;

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
		static inline void
		RegisterButtonEvent(const FButtonEventCreateInfo& _CreateInfo)
		{
			RWLock.StartWriting();
			{
				if (!ButtonEventTable.contains(_CreateInfo.Name))
				{
					VE_LOG_DEBUG("Creating a new mouse button event({}).",
						_CreateInfo.Name.GetNameWithNumber());

					auto& Slot = ButtonEventMap[UInt32(_CreateInfo.Button)]
					                                                [_CreateInfo.Action];
					ButtonEventTable[_CreateInfo.Name] = &Slot.emplace_back(_CreateInfo.Event);
				}
				else
				{
					VE_LOG_ERROR("Failed to register the mouse button event({})"
						" - duplicated event!", _CreateInfo.Name.GetNameWithNumber());
				}
			}
			RWLock.StopWriting();
		}
		static inline void
		DeleteButtonEvent(const FName& _Name)
		{
			RWLock.StartWriting();
			{
				if (ButtonEventTable.contains(_Name))
				{
					VE_LOG_DEBUG("Deleting a mouse button event({}).", _Name.GetNameWithNumber());

					auto& Exiler = ButtonEventTable[_Name];
					*Exiler = nullptr;
					ButtonEventTable.erase(_Name);
				}
				else
				{
					VE_LOG_ERROR("Failed to delete the keyboard key event({})"
						" - duplicated event!",_Name.GetNameWithNumber());
				}
			}
			RWLock.StopWriting();
		}

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

		static inline FRWLock RWLock;

		using FButtonEventTable = HashMap<FName, FButtonEvent*>;
		static inline FButtonEventTable ButtonEventTable;
		using FButtonEventMap = Segment<HashMap<EAction, Array<FButtonEvent>>, UInt32(EMouseButton::Max)>;
		static inline FButtonEventMap ButtonEventMap;

		static inline void
		ScrollCallback(GLFWwindow* window, double _OffsetX, double _OffsetY)
		{
			Scroll.Offset.x() = _OffsetX;
			Scroll.Offset.y() = _OffsetY;
		}

		static inline void
		ButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			auto& TargetEventMap = ButtonEventMap[button];
			if (TargetEventMap.contains(EAction(action)))
			{
				for (const auto& Event : TargetEventMap[EAction(action)])
				{ if (Event != nullptr) { Event(); } }
			}
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