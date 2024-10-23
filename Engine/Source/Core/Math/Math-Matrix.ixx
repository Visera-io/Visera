module;
#include <ViseraEngine>

#include <Eigen/Core>
#include <Eigen/Geometry>

export module Visera.Core.Math:Matrix;

import Visera.Internal.Memory;

export namespace VE
{

	using MatrixXF   = Eigen::MatrixX<Float>;
	using Matrix2x2F = Eigen::Matrix2<Float>;
	using Matrix3x3F = Eigen::Matrix3<Float>;
	using Matrix4x4F = Eigen::Matrix4<Float>;

	using MatrixXD   = Eigen::MatrixX<Double>;
	using Matrix2x2D = Eigen::Matrix2<Double>;
	using Matrix3x3D = Eigen::Matrix3<Double>;
	using Matrix4x4D = Eigen::Matrix4<Double>;

	using Affine3F   = Eigen::Affine3f;

	template<typename T>
	concept MatrixType = std::is_class_v<MatrixXF>   ||
						 std::is_class_v<Matrix2x2F> ||
						 std::is_class_v<Matrix3x3F> ||
						 std::is_class_v<Matrix4x4F> ||
						 std::is_class_v<MatrixXD>   ||
						 std::is_class_v<Matrix2x2D> ||
						 std::is_class_v<Matrix3x3D> ||
						 std::is_class_v<Matrix4x4D>;

	template<MatrixType T> inline
	Bool
	IsZero(const T& matrix) { return matrix.isZero(); }

	template<MatrixType T> inline
	Bool
	IsIdentity(const T& matrix) { return matrix.isIdentity(); }

	StringView
	Format(const Matrix2x2F& matrix)
	{
		static const char* formatter = "{%f, %f,\n %f, %f}";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(buffer.data(), 1 + (4 * 32) + (4-1 + 2*1) + 1, formatter,
			matrix(0,0), matrix(0,1),
			matrix(1,0), matrix(1,1));

		return StringView(buffer.begin(), buffer.begin() + cursor);
	}

	StringView
	Format(const Matrix3x3F& matrix)
	{
		static const char* formatter = "{%f, %f, %f,\n %f, %f, %f,\n %f, %f, %f}";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(buffer.data(), 1 + (9 * 32) + (9-1 + 2*2) + 1, formatter,
			matrix(0,0), matrix(0,1), matrix(0,2),
			matrix(1,0), matrix(1,1), matrix(1,2),
			matrix(2,0), matrix(2,1), matrix(2,2));

		return StringView(buffer.begin(), buffer.begin() + cursor);
	}

	StringView
	Format(const Matrix4x4F& matrix)
	{
		static const char* formatter = "{%f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f}";
		auto& buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(buffer.data(), 1 + (16 * 32) + (16-1 + 2*3) + 1, formatter,
			matrix(0,0), matrix(0,1), matrix(0,2), matrix(0,3),
			matrix(1,0), matrix(1,1), matrix(1,2), matrix(1,3),
			matrix(2,0), matrix(2,1), matrix(2,2), matrix(2,3),
			matrix(3,0), matrix(3,1), matrix(3,2), matrix(3,3));

		return StringView(buffer.begin(), buffer.begin() + cursor);
	}

} // namespace VE