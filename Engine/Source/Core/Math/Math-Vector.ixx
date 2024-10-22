module;
#include <ViseraEngine>

#include <Eigen/Core>

export module Visera.Core.Math:Vector;
import :Operation;

export namespace VE
{
	using VectorNF = Eigen::VectorX<Float>;
	using Vector2F = Eigen::Vector2<Float>;
	using Vector3F = Eigen::Vector3<Float>;
	using Vector4F = Eigen::Vector4<Float>;

	using VectorND = Eigen::VectorX<Double>;
	using Vector2D = Eigen::Vector2<Double>;
	using Vector3D = Eigen::Vector3<Double>;
	using Vector4D = Eigen::Vector4<Double>;

	template<typename T>
	concept VectorType = std::is_class_v<VectorNF> ||
						 std::is_class_v<Vector2F> ||
						 std::is_class_v<Vector3F> ||
						 std::is_class_v<Vector4F> ||
						 std::is_class_v<VectorND> ||
						 std::is_class_v<Vector2D> ||
						 std::is_class_v<Vector3D> ||
						 std::is_class_v<Vector4D>;

	template<VectorType T> inline
	T
	Unit(const T& vector) { return vector.normalized(); }

	template<VectorType T> inline
	void
	Normalize(T& vector) { vector = Unit(vector); }

	template<VectorType T> inline
	Bool
	IsUnit(T& vector) { return Equal(1.0f, vector.norm()); }

	template<VectorType T> inline
	Bool
	IsZeros(T& vector) { return vector.isZero(); }

} // namespace VE