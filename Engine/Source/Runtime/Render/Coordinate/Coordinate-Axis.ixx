module;

export module Visera.Runtime.Render.Coordinate:Axis;
import Visera.Core.Math;

export namespace VE
{

	enum class Handedness { Left, Right };
	
	union Axis
	{
		struct { const Vector3F X, Y, Z; };
		struct { const Vector3F Right, Up, Front; };
		struct { const Vector3F Pitch, Yaw, Roll; };
	};

} // namespace VE