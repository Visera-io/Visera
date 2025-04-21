module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform.IO:Keyboard;
#define VE_MODULE_NAME "IO:Keyboard"
import :Common;

import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.OS.Concurrency;

import Visera.Runtime.Platform.Window;

export namespace VE
{
	
	class Keyboard
	{
		VE_MODULE_MANAGER_CLASS(Keyboard);
	public:
		using FKeyEvent = std::function<void()>;
		struct FKeyEventCreateInfo
		{
			FName		Name;
			EKey		Key;
			EAction		Action;
			FKeyEvent	Event;
		};

	    static inline void
	    ProcessEvents();

		static inline auto
		GetKeyAction(EKey _Key) -> EAction { return EAction(glfwGetKey(Window::GetHandle(), UInt32(_Key))); }

		static inline FRWLock RWLock;

		using FKeyEventTable = HashMap<FName, FKeyEvent*>;
		static inline FKeyEventTable KeyboardEventTable;

		using FKeyEventMap = Segment<HashMap<EKey, Array<FKeyEvent>>, UInt32(EAction::Max)>;
		static inline FKeyEventMap KeyboardEventMap;

		static inline void
		RegisterKeyEvent(const FKeyEventCreateInfo& _CreateInfo)
		{
			RWLock.StartWriting();
			{
				if (!KeyboardEventTable.contains(_CreateInfo.Name))
				{
					VE_LOG_DEBUG("Creating a new keyboard key event({}).", _CreateInfo.Name.GetNameWithNumber());

					auto& Slot = KeyboardEventMap[UInt32(_CreateInfo.Action)]
					                                                  [_CreateInfo.Key];
					KeyboardEventTable[_CreateInfo.Name] = &Slot.emplace_back(_CreateInfo.Event);
				}
				else
				{
					VE_LOG_ERROR("Failed to register the keyboard key event({})"
								 " - duplicated event!", _CreateInfo.Name.GetNameWithNumber());
				}
			}
			RWLock.StopWriting();
		}

		static inline void
		DeleteKeyEvent(const FName& _Name)
		{
			RWLock.StartWriting();
			{
				if (KeyboardEventTable.contains(_Name))
				{
					VE_LOG_DEBUG("Deleting a keyboard key event({}).", _Name.GetNameWithNumber());

					auto& Exiler = KeyboardEventTable[_Name];
					*Exiler = nullptr;
					KeyboardEventTable.erase(_Name);
				}
				else
				{
					VE_LOG_ERROR("Failed to delete the keyboard key event({})"
					             " - duplicated event!", _Name.GetNameWithNumber());
				}
			}
			RWLock.StopWriting();
		}

		static inline void
		KeyCallback(GLFWwindow* window, int _Key, int _Scancode, int _Action, int _Mods)
		{
            //[TODO]: Erase expired events when processing, however, we must handle RWLock writing issues (this function was implemented within the List but now using the Array)
			RWLock.StartReading();
            {
		    	EAction Action = EAction(_Action);
            	EKey 	Key	   = EKey(_Key);
		    	switch (Action)
		    	{
		    	case EAction::Release:
            	    Action = EAction::Detach;
		    		break;
		    	case EAction::Press:
		    		break;
		    	default:
		    		VE_LOG_ERROR("Unhandled key (id:{}, action:{}, mods:{}).",
		    			         _Key, _Action, _Mods);
		    	}

				auto& TargetEventMap = KeyboardEventMap[UInt32(Action)];
				if (TargetEventMap.contains(Key))
				{
					for (const auto& Event : TargetEventMap[Key])
					{ if (Event != nullptr) { Event(); } }
				}
            }
            RWLock.StopReading();
        }
	};

	void Keyboard::
	ProcessEvents()
    {
		RWLock.StartReading();
		{
			for(const auto& [Key, Events] : KeyboardEventMap[UInt32(EAction::Hold)])
            {
            	if(EAction::Press == GetKeyAction(Key))
                {
                  for (const auto& Event : Events)
                  { if (Event != nullptr) { Event(); } }
                }
            }

            for(const auto& [Key, Events] : KeyboardEventMap[UInt32(EAction::Release)])
            {
            	if(EAction::Release == GetKeyAction(Key))
                {
                  for (const auto& Event : Events)
                  { if (Event != nullptr) { Event(); } }
                }
            }
		}
		RWLock.StopReading();
    }

} // namespace VE