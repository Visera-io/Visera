module;
#include <ViseraEngine>
#include <Eigen/Geometry>

export module Visera.Core.Math:Angle;

import :Operation;

export namespace VE
{	
	using AngleAxis   = Eigen::AngleAxisf;
	constexpr float PI { 3.141592653589793 }; // 180 Degrees

	class Degree;
	class Radian
	{
	public:
		constexpr Radian(float radian):m_radian{ radian } {};
		explicit  Radian() = default;
		explicit  Radian(Degree degree);
		Radian&   operator=(float  number) { m_radian = number; return *this; }
		Radian&   operator=(Radian radian) { m_radian = radian.m_radian; return *this; }
		Radian&   operator=(Degree degree);

		operator float() const { return m_radian; }
		operator Degree() const;
		Degree   ToDegree() const;

		Radian operator*(float target)			const;
		Radian operator*(double target)			const;
		Radian operator/(float num)				const;
		Radian operator/(double num)			const;

		bool   operator==(Radian target)	const { return Equal(  m_radian, float(target));}
		bool   operator!=(Radian target)	const { return !Equal( m_radian, float(target));}
		bool   operator<(Radian  target)	const { return m_radian < float(target);}
		bool   operator>(Radian  target)	const { return m_radian > float(target);}

	private:
		float m_radian = 0.0;
	};

	class Degree
	{
	public:
		constexpr Degree(float degree) :m_degree{ degree } {};
		explicit  Degree() = default;
		explicit  Degree(Radian radian);
		Degree& operator=(float  number) { m_degree = number; return *this; }
		Degree& operator=(Degree degree) { m_degree = degree.m_degree; return *this; }
		Degree& operator=(Radian radian);

		operator float() const { return m_degree; }
		operator Radian() const;
		Radian	 ToRadian() const;

		Radian operator*(float target)			const;
		Radian operator*(double target)			const;
		Degree operator/(float num)				const;
		Degree operator/(double num)			const;
		bool   operator==(Degree target)	const { return Equal(  m_degree, float(target));}
		bool   operator!=(Degree target)	const { return !Equal( m_degree, float(target));}
		bool   operator<(Degree  target)	const { return m_degree < float(target);}
		bool   operator>(Degree  target)	const { return m_degree < float(target);}

	private:
		float m_degree = 0.0;
	};

	float inline Tan(Radian radian)
	{
		return std::tan(radian);
	}

	float inline Tan(Degree degree)
	{
		return Tan(static_cast<Radian>(degree));
	}

	Radian inline ATan(float number)
	{
		return Radian{std::atan(number)};
	}

	Radian inline ATan(double number)
	{
		return ATan(static_cast<float>(number));
	}

	float inline Sin(Radian radian)
	{
		return std::sin(radian);
	}

	float inline Sin(Degree degree)
	{
		return Sin(static_cast<Radian>(degree));
	}

	Radian inline ASin(float number)
	{
		Assert(number > -1.0f && number < 1.0f);
		return Radian{ static_cast<float>(std::asin(Clamp(number, -1.0f, 1.0f))) };
	}

	Radian inline ASin(double number)
	{
		return ASin(static_cast<float>(number));
	}

	float inline Cos(Radian radian)
	{
		return std::cos(radian);
	}

	float inline Cos(Degree degree)
	{
		return Cos(static_cast<Radian>(degree));
	}

	Radian inline ACos(float number)
	{
		Assert(number > -1.0f && number < 1.0f);
		return Radian{ static_cast<float>(std::acos(Clamp(number, -1.0f, 1.0f))) };
	}

	Radian inline ACos(double number)
	{
		return ACos(static_cast<float>(number));
	}
	
	Degree::Degree(Radian radian):m_degree{(radian / PI) * float(180.0)} {};
	Degree& Degree::operator=(Radian radian) { m_degree = radian.ToDegree(); return *this; }
	Degree::operator Radian() const { return Radian{ (m_degree / float(180.0)) * PI }; }
	Radian Degree::ToRadian()				const { return Radian{ m_degree };}
	Radian Degree::operator*(float target)	const { return m_degree * target; }
	Radian Degree::operator*(double target)	const { return m_degree * target; }
	Degree Degree::operator/(float num)		const { return Degree(m_degree / num);	}
	Degree Degree::operator/(double num)	const { return Degree(m_degree / num);	}

	Radian::Radian(Degree degree) :m_radian{ (degree / float(180.0)) * PI } {}
	Radian& Radian::operator=(Degree degree) { m_radian = degree.ToRadian(); return *this; }
	Radian::operator Degree() const { return Degree{ (m_radian / PI) * float(180.0) }; }
	Degree Radian::ToDegree()				const { return Degree{ m_radian };}
	Radian Radian::operator*(float target)	const { return Radian(m_radian * target); }
	Radian Radian::operator*(double target)	const { return Radian(m_radian * target); }
	Radian Radian::operator/(float num)		const { return Radian(m_radian / num);	}
	Radian Radian::operator/(double num)	const { return Radian(m_radian / num);	}

} // namespace VE