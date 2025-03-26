module;
#include <Visera.h>
#include <GLFW/glfw3.h>
export module Visera.Runtime.Platform.IO:Common;

export namespace VE
{

	enum class EAction : Int32
	{
		Release = GLFW_RELEASE,	// Released
		Press   = GLFW_PRESS,	// Just Pressed
		Hold    = GLFW_REPEAT,	// Pressed and Holding
		Detach  = Hold + 1,		// Just Released  (a special Release action)
		Max		= Detach + 1,
	};
	static_assert(EAction::Press  != EAction::Hold   &&
				  EAction::Hold   != EAction::Detach &&
		          EAction::Detach != EAction::Release&&
				  UInt32(EAction::Max) == 4);

	enum class EKey : Int32
	{
		A = GLFW_KEY_A, B = GLFW_KEY_B, C = GLFW_KEY_C, D = GLFW_KEY_D,
		E = GLFW_KEY_E, F = GLFW_KEY_F, G = GLFW_KEY_G, H = GLFW_KEY_H,
		I = GLFW_KEY_I, J = GLFW_KEY_J, K = GLFW_KEY_K, L = GLFW_KEY_L,
		M = GLFW_KEY_M, N = GLFW_KEY_N, O = GLFW_KEY_O, P = GLFW_KEY_P,
		Q = GLFW_KEY_Q, R = GLFW_KEY_R, S = GLFW_KEY_S, T = GLFW_KEY_T,
		U = GLFW_KEY_U, V = GLFW_KEY_V, W = GLFW_KEY_W, X = GLFW_KEY_X,
		Y = GLFW_KEY_Y, Z = GLFW_KEY_Z,

		//Numbers
		Num0 = GLFW_KEY_0,  Num1 = GLFW_KEY_1, Num2 = GLFW_KEY_2,  
		Num3 = GLFW_KEY_3,  Num4 = GLFW_KEY_4, Num5 = GLFW_KEY_5, 
		Num6 = GLFW_KEY_6,  Num7 = GLFW_KEY_7, Num8 = GLFW_KEY_8,
		Num9 = GLFW_KEY_9, 

		//FX
		F1 = GLFW_KEY_F1,   F2 = GLFW_KEY_F2,  F3 = GLFW_KEY_F3,
		F4 = GLFW_KEY_F4,   F5 = GLFW_KEY_F5,  F6 = GLFW_KEY_F6,
		F7 = GLFW_KEY_F7,   F8 = GLFW_KEY_F8,  F9 = GLFW_KEY_F9,
		F10= GLFW_KEY_F10,  F11= GLFW_KEY_F11, F12= GLFW_KEY_F12,

		//Others
		Space  = GLFW_KEY_SPACE,
		LShift = GLFW_KEY_LEFT_SHIFT,
		RShift = GLFW_KEY_RIGHT_SHIFT,
		ESC    = GLFW_KEY_ESCAPE,
		Enter  = GLFW_KEY_ENTER,
		Tab	   = GLFW_KEY_TAB,
	};

	enum class EMouseButton : Int32
	{
		Left   = GLFW_MOUSE_BUTTON_LEFT,
		Middle = GLFW_MOUSE_BUTTON_MIDDLE,
		Right  = GLFW_MOUSE_BUTTON_RIGHT,
	};

} // namespace VE