module;
#include <Visera.h>

#include <Eigen/Core>

export module Visera.Core.Math:Vector;

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

template<VectorType T> inline
T
Unit(const T& Vector) { return Vector.normalized(); }

template<VectorType T> inline
void
Normalize(T& Vector) { Vector = Unit(Vector); }

template<VectorType T> inline
Bool
IsUnit(const T& Vector) { return Equal(1.0f, Vector.norm()); }

template<VectorType T> inline
Bool
IsZero(const T& Vector) { return Vector.isZero(); }

template<VectorType T> inline
Bool
IsIdentity(const T& Vector) { return Vector.isIdentity(); }

} // namespace VE