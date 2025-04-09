module;
#include <Visera.h>
#include <Eigen/Core>
#include <Eigen/Dense>
export module Visera.Core.Math.Basic:Vector;

import :Operation;

export namespace VE
{

	using VectorXF = Eigen::VectorX<Float>;
	using Vector2F = Eigen::Vector2<Float>;
	using Vector3F = Eigen::Vector3<Float>;
	using Vector4F = Eigen::Vector4<Float>;

	using VectorXD = Eigen::VectorX<Double>;
	using Vector2D = Eigen::Vector2<Double>;
	using Vector3D = Eigen::Vector3<Double>;
	using Vector4D = Eigen::Vector4<Double>;

	template<typename T>
	concept VectorType = std::is_class_v<VectorXF> ||
							std::is_class_v<Vector2F> ||
							std::is_class_v<Vector3F> ||
							std::is_class_v<Vector4F> ||
							std::is_class_v<VectorXD> ||
							std::is_class_v<Vector2D> ||
							std::is_class_v<Vector3D> ||
							std::is_class_v<Vector4D>;

	template<VectorType T>
	T
	Unit(const T& Vector) { return Vector.normalized(); }

	template<VectorType T>
	void
	Normalize(T& Vector) { Vector = Unit(Vector); }

	template<VectorType T>
	Bool
	IsUnit(const T& Vector) { return Equal(1.0f, Vector.norm()); }

	template<VectorType T>
	Bool
	IsZero(const T& Vector) { return Vector.isZero(); }

	template<VectorType T>
	Bool
	IsIdentity(const T& Vector) { return Vector.isIdentity(); }

	//Even though this looks like it returns a new vector, Eigen is smart: if a is on both sides, Eigen avoids allocating a temporary and does it in-place behind the scenes (thanks to expression templates and lazy evaluation).
	template<VectorType T>
	T
	GetComponentWiseMax(const T& _A, const T& _B) { return _A.cwiseMax(_B); }

	//Even though this looks like it returns a new vector, Eigen is smart: if a is on both sides, Eigen avoids allocating a temporary and does it in-place behind the scenes (thanks to expression templates and lazy evaluation).
	template<VectorType T>
	T
	GetComponentWiseMin(const T& _A, const T& _B) { return _A.cwiseMin(_B); }

} // namespace VE