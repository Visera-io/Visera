module;

export module Visera.Core.Base.Coordinate:World;
import :Axis;
import Visera.Core.Math;

export namespace VE
{
	/* [World Coordinate]
	 * Example:
	 * 1. World::Origin
	 * 2. World::Axis.X / World::Axis.Right / World::Axis.Pitch
	 */
	class World
	{
	public:
		static constexpr auto
			Handedness = Handedness::Left;

		static inline const Vector3F
			Origin{0, 0, 0};
		
		static inline const Axis
			Axis
			{ {
				.X {1, 0, 0},
				.Y {0, 1, 0},
				.Z {0, 0, 1},
			} };

	private:
		World()							 = delete;
		World(const World&)				 = delete;
		World(World&&)					 = delete;
		World& operator = (const World&) = delete;
		World& operator = (World&&)		 = delete;
	};
} // namespace VE