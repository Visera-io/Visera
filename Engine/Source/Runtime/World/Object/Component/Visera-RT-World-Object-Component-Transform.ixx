module;
#include <Visera.h>
export module Visera.Runtime.World.Object.Component:Transform;

import :Interface;
import Visera.Core.Math.Basic;

export namespace VE
{
	class OCTransform : public IObjectComponent
	{
	public:
		//auto GetMatrix()		const   -> const Matrix4x4F& { return Affine.matrix(); }

		auto GetScaling()		const	-> const Vector4F&	 { return Scaling; }
		auto GetRotation()		const	-> const Vector4F&	 { return Rotation; }
		auto GetTranslation()	const	-> const Vector4F&	 { return Translation; }

		/*void SetScaling(Vector3F _NewScale)			{ Scaling		= std::move(_NewScale);		Matrix.reset(); }
		void SetRotation(Vector3F _NewRotate)		{ Rotation		= std::move(_NewRotate);	Matrix.reset(); }
		void SetTranslation(Vector3F _NewTranslate)	{ Translation	= std::move(_NewTranslate); Matrix.reset(); }*/

		//void Scale(Float _X, Float _Y, Float _Z)	{ SetScaling({ _X, _Y, _Z }); }
		//void Rotate();
		//void Translate();

	protected:
		virtual void Update()  override { UpdateMatrix(); };
		virtual void Create()  override {};
		virtual void Destroy() override {};

	public:
		OCTransform() : IObjectComponent(FName{"component::transform"}) {};

	private:
		Optional<Matrix4x4F> Matrix;
		Vector4F			 Scaling;
		Vector4F			 Rotation;
		Vector4F			 Translation;

		void UpdateMatrix() { if (!Matrix.has_value()) { VE_WIP; }; }
	};

} // namespace VE