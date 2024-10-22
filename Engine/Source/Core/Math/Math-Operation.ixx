module;
#include <ViseraEngine>

export module Visera.Core.Math:Operation;

export namespace VE
{
	template<FloatingPoint T>
	inline auto Epsilon() { return std::numeric_limits<T>::epsilon(); }

	template<Integer T>
	inline auto Epsilon() { return 1; }

	template<Number T>
	inline auto Infinity() { return std::numeric_limits<T>::infinity(); }

	template<FloatingPoint T>
	Bool Equal(T a, T b) { return std::abs(a - b) <= Epsilon<T>(); }

	template<Number NumT, Number FloorT, Number CeilT>
	Bool Clamp(NumT number, FloorT floor, CeilT ceil)
	{
		if (floor > number) return floor;
		if (ceil  < number) return ceil;
		return number;
	}
} // namespace VE
