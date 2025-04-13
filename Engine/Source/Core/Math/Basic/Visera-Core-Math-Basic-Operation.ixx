module;
#include <Visera.h>
export module Visera.Core.Math.Basic:Operation;

export namespace VE
{
	template<FloatingPoint T> constexpr auto
	Epsilon() { return std::numeric_limits<T>::epsilon(); }

	template<Integer T> constexpr auto
	Epsilon() { return 1; }

	template<Number T> constexpr auto
	UpperBound() { return std::numeric_limits<T>::max(); }

	template<Number T> constexpr auto
	LowerBound() { return std::numeric_limits<T>::min(); }

	template<FloatingPoint T> Bool
	Equal(T A, T B) { return std::abs(A - B) <= Epsilon<T>(); }

	template<Number NumT, Number FloorT, Number CeilT>
	void Clamp(NumT* _Value_, FloorT _Floor, CeilT _Ceil)
	{
		if (_Floor > *_Value_) { *_Value_ = _Floor; return; }
		if (_Ceil  < *_Value_) { *_Value_ = _Ceil;  return; }
	}

	template<Number NumT, Number FloorT, Number CeilT>
	NumT GetClamped(NumT _Value, FloorT _Floor, CeilT _Ceil)
	{
		if (_Floor > _Value) return _Floor;
		if (_Ceil  < _Value) return _Ceil;
		return _Value;
	}

} // namespace VE
