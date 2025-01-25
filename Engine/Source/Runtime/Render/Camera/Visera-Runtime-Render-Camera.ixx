module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera;

import Visera.Core.Math;

export namespace VE { namespace Runtime
{
	
	class VCamera
	{
	public:
		enum class EMode
		{ Orthographic, Perspective , Default = Perspective };

		auto GetViewingMatrix() const -> const Matrix4x4F& { return ViewingMatrix; }
		auto GetProjectMatrix() const -> const Matrix4x4F& { return ProjectMatrix; }

		VCamera(EMode _Mode);

	private:
		EMode      Mode = EMode::Default;
		Matrix4x4F ViewingMatrix;
		Matrix4x4F ProjectMatrix;
	};

	VCamera::
	VCamera(EMode _Mode)
	{

	}

} } // namespace VE::Runtime