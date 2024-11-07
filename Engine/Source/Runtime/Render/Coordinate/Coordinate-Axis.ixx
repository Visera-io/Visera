module;

export module Visera.Engine.Runtime.Render.Coordinate:Axis;
import Visera.Engine.Core.Math;

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