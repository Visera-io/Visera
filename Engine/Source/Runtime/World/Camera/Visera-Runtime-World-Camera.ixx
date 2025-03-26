module;
#include <Visera.h>
export module Visera.Runtime.World.Camera;
import Visera.Runtime.World.Camera.Lens;
import Visera.Runtime.World.Camera.Film;

import Visera.Core.Math.Basic;
import Visera.Runtime.World.Object;
import Visera.Runtime.World.Atlas;

export namespace VE
{
	
	class VCamera : VObject
	{
	public:
		enum class EMode
		{ Orthographic, Perspective , Default = Perspective };

		auto GetViewingMatrix() const -> const Matrix4x4F& { return ViewingMatrix; }
		auto GetProjectMatrix() const -> const Matrix4x4F& { return ProjectMatrix; }

		VCamera(EMode _Mode = EMode::Default);

		virtual void Create()	override;
		virtual void Destroy()	override;

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Forward= Atlas::Visera.Forward;
		Matrix4x4F ViewingMatrix;
		Matrix4x4F ProjectMatrix;

		FDiskLens  Lens{1.0};
		FRawFilm   Film{{1600, 900}};
	};

	void VCamera::Create()
	{
		AttachTransformComponent();
	}

	void VCamera::Destroy()
	{

	}

	VCamera::
	VCamera(EMode _Mode/* = EMode::Default */) : Mode {_Mode}
	{

	}

} // namespace VE