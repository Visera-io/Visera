module;
#include <ViseraEngine>

#include <Eigen/Core>

export module Visera.Core.Math:Vector;

import Visera.Internal.Memory;

import :Operation;

export namespace VE
{
	using VectorXF = Eigen::VectorX<Float>;
	using Vector2F = Eigen::Vector2<Float>;
	using Vector3F = Eigen::Vector3<Float>;
	using Vector4F = Eigen::Vector4<Float>;

	using VectorND = Eigen::VectorX<Double>;
	using Vector2D = Eigen::Vector2<Double>;
	using Vector3D = Eigen::Vector3<Double>;
	using Vector4D = Eigen::Vector4<Double>;

	template<typename T>
	concept VectorType = std::is_class_v<VectorXF> ||
						 std::is_class_v<Vector2F> ||
						 std::is_class_v<Vector3F> ||
						 std::is_class_v<Vector4F> ||
						 std::is_class_v<VectorND> ||
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

	StringView
	Format(const Vector2F& Vector)
	{
		static const char* formatter = "(%f, %f)";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatVector;
		
		auto cursor = std::snprintf(buffer.data(), 32 * 2 + (1 + 2*1 + 1), formatter, Vector[0], Vector[1]);
		return StringView(buffer.begin(), buffer.begin() + cursor);
	}

	StringView
	Format(const Vector3F& Vector)
	{
		static const char* formatter = "(%f, %f, %f)";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatVector;

		auto cursor = std::snprintf(buffer.data(), 32 * 3 + (1 + 2*2 + 1), formatter, Vector[0], Vector[1], Vector[2]);
		return StringView(buffer.begin(), buffer.begin() + cursor);
	}

	StringView
	Format(const Vector4F& Vector)
	{
		static const char* formatter = "(%f, %f, %f, %f)";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatVector;

		auto cursor = std::snprintf(buffer.data(), 32 * 4 + (1 + 2*3 + 1), formatter, Vector[0], Vector[1], Vector[2], Vector[3]);
		return StringView(buffer.begin(), buffer.begin() + cursor);
	}
} // namespace VE