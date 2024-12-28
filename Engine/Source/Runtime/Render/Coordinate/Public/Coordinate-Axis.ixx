module;
#include <Visera.h>
export module Visera.Runtime.Render.Coordinate:Axis;
import Visera.Core.Math;

VISERA_PUBLIC_MODULE

enum class Handedness { Left, Right };
	
union Axis
{
	struct { const Vector3F X, Y, Z; };
	struct { const Vector3F Right, Up, Front; };
	struct { const Vector3F Pitch, Yaw, Roll; };
};

VISERA_MODULE_END