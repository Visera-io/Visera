module;
#include <Visera.h>
export module Visera.Core.Math.Basic;
export import :Angle;
export import :Vector;
export import :Matrix;
export import :Operation;

export namespace VE
{
    constexpr Float E = 2.71828182845904523536028747135266250;

    template <Number NumT> NumT
    Sqrt(NumT _Num) { return std::sqrt(_Num); }

    String inline
    Text(const	Vector2F& Vector)	{ return Text("[{}, {}]", Vector[0], Vector[1]); }
    String inline
    Text(const	Vector3F& Vector)	{ return Text("[{}, {}, {}]", Vector[0], Vector[1], Vector[2]); }
    String inline
    Text(const	Vector4F& Vector)	{ return Text("[{}, {}, {}, {}]", Vector[0], Vector[1], Vector[2], Vector[3]); }
    String inline
    Text(const	Vector2D& Vector)	{ return Text("[{}, {}]", Vector[0], Vector[1]); }
    String inline
    Text(const	Vector3D& Vector)	{ return Text("[{}, {}, {}]", Vector[0], Vector[1], Vector[2]); }
    String inline
    Text(const	Vector4D& Vector)	{ return Text("[{}, {}, {}, {}]", Vector[0], Vector[1], Vector[2], Vector[3]); }
    String inline
    Text(const	Radian&	  Radian)	{ return Text("{}rad",	Float(Radian)); }
    String inline
    Text(const	Degree&	  Degree)	{ return Text("{}deg",	Float(Degree)); }
    String inline
    Text(const	VectorXF& Vector) { String Result = "["; for (const auto& Value : Vector) { Result += Text(" {},", Value); } Result.pop_back(); return Result += " ]"; }
    String inline
    Text(const	VectorXD& Vector) { String Result = "["; for (const auto& Value : Vector) { Result += Text(" {},", Value); } Result.pop_back(); return Result += " ]"; }
    String inline
    Text(const MatrixXF Matrix)
    {
        String Result;
        UInt32 Rows = Matrix.rows();
        UInt32 Cols = Matrix.cols();
        for (UInt32 i = 0; i < Rows; ++i)
        {
            Result += '|';
            for (UInt32 j = 0; j < Cols; ++j)
            { Result += Text(" {:<10.6f},", Matrix(i, j)); }
            Result.pop_back();  //Pop Last ','
            Result += " |\n";
        }
        Result.pop_back(); //Pop Last '\n'
        return Result += Text("_Matrix{}x{}", Rows, Cols);
    }
    String inline
    Text(const Matrix2x2F& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f} |_Matrix2x2",
            Matrix(0, 0), Matrix(0, 1),
            Matrix(1, 0), Matrix(1, 1));
    }
    String inline
    Text(const Matrix2x2D& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f} |_Matrix2x2",
            Matrix(0, 0), Matrix(0, 1),
            Matrix(1, 0), Matrix(1, 1));
    }
    String inline
    Text(const Matrix3x3F& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f} |_Matrix3x3",
            Matrix(0, 0), Matrix(0, 1), Matrix(0, 2),
            Matrix(1, 0), Matrix(1, 1), Matrix(1, 2),
            Matrix(2, 0), Matrix(2, 1), Matrix(2, 2));
    }
    String inline
    Text(const Matrix3x3D& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f} |_Matrix3x3",
            Matrix(0, 0), Matrix(0, 1), Matrix(0, 2),
            Matrix(1, 0), Matrix(1, 1), Matrix(1, 2),
            Matrix(2, 0), Matrix(2, 1), Matrix(2, 2));
    }
    String inline
    Text(const Matrix4x4F& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |_Matrix4x4",
            Matrix(0, 0), Matrix(0, 1), Matrix(0, 2), Matrix(0, 3),
            Matrix(1, 0), Matrix(1, 1), Matrix(1, 2), Matrix(1, 3),
            Matrix(2, 0), Matrix(2, 1), Matrix(2, 2), Matrix(2, 3),
            Matrix(3, 0), Matrix(3, 1), Matrix(3, 2), Matrix(3, 3));
    }
    String inline
    Text(const Matrix4x4D& Matrix)
    {
        return Text("| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |\n"
                    "| {:<10.6f}, {:<10.6f}, {:<10.6f}, {:<10.6f} |_Matrix4x4",
            Matrix(0, 0), Matrix(0, 1), Matrix(0, 2), Matrix(0, 3),
            Matrix(1, 0), Matrix(1, 1), Matrix(1, 2), Matrix(1, 3),
            Matrix(2, 0), Matrix(2, 1), Matrix(2, 2), Matrix(2, 3),
            Matrix(3, 0), Matrix(3, 1), Matrix(3, 2), Matrix(3, 3));
    }
} // namespace VE