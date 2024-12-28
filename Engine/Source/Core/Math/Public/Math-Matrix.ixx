module;
#include <Visera.h>

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
	IsZero(const T& Matrix) { return Matrix.isZero(); }

	template<MatrixType T> inline
	Bool
	IsIdentity(const T& Matrix) { return Matrix.isIdentity(); }

	StringView
	Format(const Matrix2x2F& Matrix)
	{
		static const char* Formatter = "{%f, %f,\n %f, %f}";
		auto& Buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(Buffer.data(), 1 + (4 * 32) + (4-1 + 2*1) + 1, Formatter,
			Matrix(0,0), Matrix(0,1),
			Matrix(1,0), Matrix(1,1));

		return StringView(Buffer.begin(), Buffer.begin() + cursor);
	}

	StringView
	Format(const Matrix3x3F& Matrix)
	{
		static const char* Formatter = "{%f, %f, %f,\n %f, %f, %f,\n %f, %f, %f}";
		auto& Buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(Buffer.data(), 1 + (9 * 32) + (9-1 + 2*2) + 1, Formatter,
			Matrix(0,0), Matrix(0,1), Matrix(0,2),
			Matrix(1,0), Matrix(1,1), Matrix(1,2),
			Matrix(2,0), Matrix(2,1), Matrix(2,2));

		return StringView(Buffer.begin(), Buffer.begin() + cursor);
	}

	StringView
	Format(const Matrix4x4F& Matrix)
	{
		static const char* Formatter = "{%f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f}";
		auto& Buffer = MemoryRegistry::GetInstance().MemFormatMatrix;

		auto cursor = std::snprintf(Buffer.data(), 1 + (16 * 32) + (16-1 + 2*3) + 1, Formatter,
			Matrix(0,0), Matrix(0,1), Matrix(0,2), Matrix(0,3),
			Matrix(1,0), Matrix(1,1), Matrix(1,2), Matrix(1,3),
			Matrix(2,0), Matrix(2,1), Matrix(2,2), Matrix(2,3),
			Matrix(3,0), Matrix(3,1), Matrix(3,2), Matrix(3,3));

		return StringView(Buffer.begin(), Buffer.begin() + cursor);
	}

VISERA_MODULE_END