module;

export module Visera.App.Math;
import Visera.App.Base;

import Visera.Engine.Core.Math;

export namespace VISERA_APP_NAMESPACE
{
	//Angel
	constexpr double PI = VE::PI;
	using Radian	= VE::Radian;
	using Degree	= VE::Degree;

	//Vector
	using VectorXF = VE::VectorXF;
	using Vector2F = VE::Vector2F;
	using Vector3F = VE::Vector3F;
	using Vector4F = VE::Vector4F;

	using VectorND = VE::VectorND;
	using Vector2D = VE::Vector2D;
	using Vector3D = VE::Vector3D;
	using Vector4D = VE::Vector4D;

	//Matrix
	using MatrixXF   = VE::MatrixXF;
	using Matrix2x2F = VE::Matrix2x2F;
	using Matrix3x3F = VE::Matrix3x3F;
	using Matrix4x4F = VE::Matrix4x4F;

	using MatrixXD   = VE::MatrixXD;
	using Matrix2x2D = VE::Matrix2x2D;
	using Matrix3x3D = VE::Matrix3x3D;
	using Matrix4x4D = VE::Matrix4x4D;

	using Affine3F   = VE::Affine3F;

	//Operation
	template<FloatingPoint T>
	inline auto Epsilon() { return VE::Epsilon<T>(); }

	template<Integer T>
	inline auto Epsilon() { return VE::Epsilon<T>(); }

	template<Number T>
	inline auto UpperBound() { return VE::UpperBound<T>(); }

	template<Number T>
	inline auto LowerBound() { return VE::LowerBound<T>(); }

	template<FloatingPoint T>
	Bool Equal(T A, T B) { return VE::Equal(A, B); }

	template<Number NumT, Number FloorT, Number CeilT>
	void Clamp(NumT* Value, FloorT Floor, CeilT Ceil) { return VE::Clamp(Value, Floor, Ceil); }

	Radian inline	Tan(Radian Radian)	{ return VE::Tan(Radian); }
	Degree inline	Tan(Degree Degree)	{ return VE::Tan(Degree); }

	Float inline	ATan(Float Value)	{ return VE::ATan(Value); }
	Double inline	ATan(Double Value)	{ return VE::ATan(Value); }

	Radian inline	Sin(Radian Radian)	{ return VE::Sin(Radian); }
	Degree inline	Sin(Degree Degree)	{ return VE::Sin(Degree); }

	Float	inline	ASin(Float Value)	{ return VE::ASin(Value); }
	Double	inline	ASin(Double Value)	{ return VE::ASin(Value); }
	Float	inline	ASinSafe(Float Value)	{ Clamp(&Value, -1.0f, 1.0f); return VE::ASin(Value); }
	Double	inline	ASinSafe(Double Value)	{ Clamp(&Value, -1.0f, 1.0f); return VE::ASin(Value); }

	Radian inline	Cos(Radian Radian)	{ return VE::Cos(Radian); }
	Degree inline	Cos(Degree Degree)	{ return VE::Cos(Degree); }

	Float	inline	ACos(Float Value)	{ return VE::ACos(Value); }
	Double	inline	ACos(Double Value)	{ return VE::ACos(Value); }
	Float	inline	ACosSafe(Float Value)	{ Clamp(&Value, -1.0f, 1.0f); return VE::ACos(Value); }
	Double	inline	ACosSafe(Double Value)	{ Clamp(&Value, -1.0f, 1.0f); return VE::ACos(Value); }
	
} // namespace VISERA_APP_NAMESPACE