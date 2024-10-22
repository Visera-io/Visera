module;
#include <ViseraEngine>

#include <Eigen/Core>
#include <Eigen/Geometry>

export module Visera.Core.Math:Matrix;

export namespace VE
{

	using Matrix	= Eigen::MatrixX<Float>;
	using Matrix2x2 = Eigen::Matrix2<Float>;
	using Matrix3x3 = Eigen::Matrix3<Float>;
	using Matrix4x4	= Eigen::Matrix4<Float>;
	using Affine3F  = Eigen::Affine3f;

} // namespace VE