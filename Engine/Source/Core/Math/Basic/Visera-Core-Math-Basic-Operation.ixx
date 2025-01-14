module;
#include <Visera.h>

export module Visera.Core.Math.Basic:Operation;

export namespace VE
{
	template<FloatingPoint T>
	inline auto Epsilon() { return std::numeric_limits<T>::epsilon(); }

	template<Integer T>
	inline auto Epsilon() { return 1; }

	template<Number T>
	inline auto UpperBound() { return std::numeric_limits<T>::max(); }

	template<Number T>
	inline auto LowerBound() { return std::numeric_limits<T>::min(); }

	template<FloatingPoint T>
	Bool Equal(T A, T B) { return std::abs(A - B) <= Epsilon<T>(); }

	template<Number NumT, Number FloorT, Number CeilT>
	void Clamp(NumT* Value, FloorT Floor, CeilT Ceil)
	{
		if (Floor > *Value) { *Value = Floor; return; }
		if (Ceil  < *Value) { *Value = Ceil;  return; }
	}

} // namespace VE
