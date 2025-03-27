module;
#include <Visera.h>
#include <tbb/tbb.h>
export module Visera.Runtime.World.Camera;
export import Visera.Runtime.World.Camera.Lens;
export import Visera.Runtime.World.Camera.Film;

import Visera.Core.Math.Basic;
import Visera.Runtime.World.Atlas;
import Visera.Runtime.World.Ray;
import Visera.Runtime.World.Stage;

export namespace VE
{
	
	class VCamera
	{
	public:
		enum class EMode
		{ Orthographic, Perspective , Default = Perspective };

		void Shoot(SharedPtr<const FScene> _Scene);
		auto GetFilm() const -> SharedPtr<const IFilm> { return Film; }

		auto GetViewingMatrix() const -> const Matrix4x4F& { return ViewingMatrix; }
		auto GetProjectMatrix() const -> const Matrix4x4F& { return ProjectMatrix; }

		VCamera() = delete;
		VCamera(SharedPtr<ILens> _Lens, SharedPtr<IFilm> _Film, EMode _Mode = EMode::Default);

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Forward= Atlas::Visera.Forward;

		Matrix4x4F ViewingMatrix;
		Matrix4x4F ProjectMatrix;

		SharedPtr<ILens>  Lens;
		SharedPtr<IFilm>  Film;
	};

	void VCamera::
	Shoot(SharedPtr<const FScene> _Scene)
	{
		//[FIXME]: Testing

		//tbb::parallel_for()
	}

	VCamera::
	VCamera(SharedPtr<ILens> _Lens,
			SharedPtr<IFilm> _Film,
			EMode _Mode/* = EMode::Default */)
			:
			Lens{ std::move(_Lens) },
			Film{std::move(_Film)},
			Mode {_Mode}
	{

	}

} // namespace VE