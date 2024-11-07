module;
#include <Visera>
#include <Eigen/Geometry>

export module Visera.Engine.Core.Math:Angle;

import :Operation;

export namespace VE
{	
	using AngleAxis   = Eigen::AngleAxisf;
	constexpr Double PI { 3.141592653589793 }; // 180 Degrees

	class Degree;
	class Radian
	{
	public:
		constexpr Radian(Float Value):Value{ Value } {};
		explicit  Radian() = default;
		explicit  Radian(Degree TargetDegree);
		Radian&   operator=(Float  NewValue) { Value = NewValue; return *this; }
		Radian&   operator=(Radian NewRadian) { Value = NewRadian.Value; return *this; }
		Radian&   operator=(Degree TargetDegree);

		operator Float() const { return Value; }
		operator Degree() const;
		Degree   ToDegree() const;
		String	 ToString() const { return std::format("{} rad", Value); }

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
		constexpr Degree(Float Value) :Value{ Value } {};
		explicit  Degree() = default;
		explicit  Degree(Radian TargetRadian);
		Degree& operator=(Float  NewValue) { Value = NewValue; return *this; }
		Degree& operator=(Degree NewDegree) { Value = NewDegree.Value; return *this; }
		Degree& operator=(Radian TargetRadian);

		operator Float() const { return Value; }
		operator Radian() const;
		Radian	 ToRadian() const;
		String	 ToString() const { return std::format("{}°", Value); }

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

	Radian inline Tan(Radian Radian)
	{
		return std::tan(Radian);
	}

	Degree inline Tan(Degree Degree)
	{
		return Tan(static_cast<Radian>(Degree));
	}

	Float inline ATan(Float Value)
	{
		return Radian{std::atan(Value)};
	}

	Double inline ATan(Double Value)
	{
		return ATan(static_cast<Float>(Value));
	}

	Radian inline Sin(Radian Radian)
	{
		return std::sin(Radian);
	}

	Degree inline Sin(Degree Degree)
	{
		return Sin(static_cast<Radian>(Degree));
	}

	Float inline ASin(Float Value)
	{
		Assert(Value >= -1.0f && Value <= 1.0f);
		return Radian{ static_cast<Float>(std::asin(Value)) };
	}

	Double inline ASin(Double Value)
	{
		return ASin(static_cast<Float>(Value));
	}

	Radian inline Cos(Radian Radian)
	{
		return std::cos(Radian);
	}

	Degree inline Cos(Degree Degree)
	{
		return Cos(static_cast<Radian>(Degree));
	}

	Float inline ACos(Float Value)
	{
		Assert(Value >= -1.0f && Value <= 1.0f);
		return Radian{ static_cast<Float>(std::acos(Value)) };
	}

	Double inline ACos(Double Value)
	{
		return ACos(static_cast<Float>(Value));
	}
	
	Degree::Degree(Radian TargetRadian):Value{(TargetRadian / PI) * 180.0f} {};
	Degree& Degree::operator=(Radian TargetRadian) { Value = TargetRadian.ToDegree(); return *this; }
	Degree::operator Radian() const { return Radian{ (Value / 180.0f) * Float(PI) }; }
	Radian Degree::ToRadian()						const { return Radian{ Value };}
	Radian Degree::operator*(Float Multiplicand)	const { return Value * Multiplicand; }
	Radian Degree::operator*(Double Multiplicand)	const { return Value * Multiplicand; }
	Degree Degree::operator/(Float Divisor)			const { return Degree(Value / Divisor);	}
	Degree Degree::operator/(Double Divisor)		const { return Degree(Value / Divisor);	}

	Radian::Radian(Degree TargetDegree) :Value{ (TargetDegree / 180.0f) * Float(PI) } {}
	Radian& Radian::operator=(Degree TargetDegree) { Value = TargetDegree.ToRadian(); return *this; }
	Radian::operator Degree() const { return Degree{ Float(Value / PI) * 180.0f }; }
	Degree Radian::ToDegree()						const { return Degree{ Value };}
	Radian Radian::operator*(Float Multiplicand)	const { return Radian(Value * Multiplicand); }
	Radian Radian::operator*(Double Multiplicand)	const { return Radian(Value * Multiplicand); }
	Radian Radian::operator/(Float Divisor)			const { return Radian(Value / Divisor);	}
	Radian Radian::operator/(Double Divisor)		const { return Radian(Value / Divisor);	}

} // namespace VE