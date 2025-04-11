module;
#include <Visera.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

export module Visera.Core.Math.Basic:Matrix;

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

	Matrix3x3F inline
	ComputeCofactorMatrix(const Matrix3x3F& _Matrix, Matrix3x3F* InverseMatrix_ = nullptr, Float* Determinant_ = nullptr)
	{
		const Eigen::FullPivLU<Matrix3x3F> LUDecomposition(_Matrix);

		auto M_inverse = LUDecomposition.inverse();  // This avoids recalculating the inverse multiple times
		if (InverseMatrix_) { *InverseMatrix_ = M_inverse; }
		Float M_determinant = LUDecomposition.determinant();  // Efficient determinant computation
		if (Determinant_) { *Determinant_ = M_determinant; }

		return M_determinant * M_inverse.transpose();
	}

	Matrix3x3F inline
	ComputeCofactorMatrixFromHomogeneous(const Matrix4x4F& _HomogeneousMatrix, Matrix3x3F* InverseMatrix_ = nullptr, Float* Determinant_ = nullptr)
	{
		return ComputeCofactorMatrix(_HomogeneousMatrix.block<3,3>(0,0), InverseMatrix_, Determinant_);
	}

} // namespace VE