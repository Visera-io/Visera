module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera;

import Visera.Core.Math;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Atlas;

export namespace VE { namespace Runtime
{
	
	class VCamera : VObject
	{
	public:
		enum class EMode
		{ Orthographic, Perspective , Default = Perspective };

		auto GetViewingMatrix() const -> const Matrix4x4F& { return ViewingMatrix; }
		auto GetProjectMatrix() const -> const Matrix4x4F& { return ProjectMatrix; }

		VCamera(EMode _Mode);

		virtual void Create()	override;
		virtual void Destroy()	override;

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Forward= Atlas::Visera.Forward;
		Matrix4x4F ViewingMatrix;
		Matrix4x4F ProjectMatrix;
	};

	void VCamera::Create()
	{
		AttachTransformComponent();
	}

	void VCamera::Destroy()
	{

	}

	VCamera::
	VCamera(EMode _Mode) : Mode {_Mode}
	{

	}

} } // namespace VE::Runtime