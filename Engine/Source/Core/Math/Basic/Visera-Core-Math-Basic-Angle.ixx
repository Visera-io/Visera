module;
#include <Visera.h>
#include <Eigen/Geometry>

export module Visera.Core.Math.Basic:Angle;

import :Operation;

export namespace VE
{	
	using AngleAxis   = Eigen::AngleAxisf;
	constexpr Float PI { 3.14159265358979323846264338327950288 };

	class Degree;
	class Radian
	{
	public:
		Float& Data() { return Value; }

		constexpr Radian(Float Value):Value{ Value } {};
		explicit  Radian() = default;
		explicit  Radian(Degree TargetDegree);
		Radian&   operator=(Float  NewValue) { Value = NewValue; return *this; }
		Radian&   operator=(Radian NewRadian) { Value = NewRadian.Value; return *this; }
		Radian&   operator=(Degree TargetDegree);

		operator Float() const { return Value; }
		operator Degree() const;
		Degree   ToDegree() const;

		Radian operator*(Float Multiplicand)	const;
		Radian operator*(Double Multiplicand)	const;
		Radian operator/(Float Divisor)			const;
		Radian operator/(Double Divisor)		const;

		bool   operator==(Radian Rival)	const { return Equal(  Value, Rival.Value);}
		bool   operator!=(Radian Rival)	const { return !Equal( Value, Rival.Value);}
		bool   operator<(Radian  Rival)	const { return Value < Rival.Value;}
		bool   operator>(Radian  Rival)	const { return Value > Rival.Value;}

	private:
		Float Value = 0.0;
	};

	class Degree
	{
	public:
		Float& Data() {return Value; }

		constexpr Degree(Float Value) :Value{ Value } {};
		explicit  Degree() = default;
		explicit  Degree(Radian TargetRadian);
		Degree& operator=(Float  NewValue) { Value = NewValue; return *this; }
		Degree& operator=(Degree NewDegree) { Value = NewDegree.Value; return *this; }
		Degree& operator=(Radian TargetRadian);

		operator Float() const { return Value; }
		operator Radian() const;
		Radian	 ToRadian() const;

		Radian operator*(Float Multiplicand)	const;
		Radian operator*(Double Multiplicand)	const;
		Degree operator/(Float Divisor)			const;
		Degree operator/(Double Divisor)		const;
		bool   operator==(Degree Rival)	const { return Equal(  Value, Rival.Value);}
		bool   operator!=(Degree Rival)	const { return !Equal( Value, Rival.Value);}
		bool   operator<(Degree  Rival)	const { return Value < Rival.Value;}
		bool   operator>(Degree  Rival)	const { return Value < Rival.Value;}

	private:
		Float Value = 0.0;
	};

	Float inline Tan(Radian _Radian)
	{
		return std::tan(_Radian);
	}

	Float inline Tan(Degree _Degree)
	{
		return Tan(_Degree.ToRadian());
	}

	Radian inline ATan(Float _Value)
	{
		return Radian{std::atan(_Value)};
	}

	Radian inline ATan(Double _Value)
	{
		return ATan(static_cast<Float>(_Value));
	}

	Float inline Sin(Radian _Radian)
	{
		return std::sin(_Radian);
	}

	Degree inline Sin(Degree _Degree)
	{
		return Sin(_Degree.ToRadian());
	}

	Radian inline ASin(Float _Value)
	{
		VE_ASSERT(_Value >= -1.0f && _Value <= 1.0f);
		return Radian{ static_cast<Float>(std::asin(_Value)) };
	}

	Radian inline ASin(Double _Value)
	{
		return ASin(static_cast<Float>(_Value));
	}

	Float inline Cos(Radian _Radian)
	{
		return std::cos(_Radian);
	}

	Float inline Cos(Degree _Degree)
	{
		return Cos(_Degree.ToRadian());
	}

	Radian inline ACos(Float _Value)
	{
		VE_ASSERT(_Value >= -1.0f && _Value <= 1.0f);
		return Radian{ static_cast<Float>(std::acos(_Value)) };
	}

	Radian inline ACos(Double _Value)
	{
		return Radian{ ACos(static_cast<Float>(_Value)) };
	}

	Degree::Degree(Radian TargetRadian):Value{(TargetRadian / PI) * 180.0f} {};
	Degree& Degree::operator=(Radian TargetRadian) { Value = TargetRadian.ToDegree(); return *this; }
	Degree::operator Radian() const { return Radian{ (Value / 180.0f) * Float(PI) }; }
	Radian Degree::ToRadian()						const { return Radian{ (Value / 180.0f) * Float(PI) }; }
	Radian Degree::operator*(Float Multiplicand)	const { return Value * Multiplicand; }
	Radian Degree::operator*(Double Multiplicand)	const { return Value * Multiplicand; }
	Degree Degree::operator/(Float Divisor)			const { return Degree(Value / Divisor);	}
	Degree Degree::operator/(Double Divisor)		const { return Degree(Value / Divisor);	}

	Radian::Radian(Degree TargetDegree) :Value{ (TargetDegree / 180.0f) * Float(PI) } {}
	Radian& Radian::operator=(Degree TargetDegree) { Value = TargetDegree.ToRadian(); return *this; }
	Radian::operator Degree() const { return Degree{ Float(Value / PI) * 180.0f }; }
	Degree Radian::ToDegree()						const { return Degree{ Float(Value / PI) * 180.0f }; }
	Radian Radian::operator*(Float Multiplicand)	const { return Radian(Value * Multiplicand); }
	Radian Radian::operator*(Double Multiplicand)	const { return Radian(Value * Multiplicand); }
	Radian Radian::operator/(Float Divisor)			const { return Radian(Value / Divisor);	}
	Radian Radian::operator/(Double Divisor)		const { return Radian(Value / Divisor);	}

} // namespace VE