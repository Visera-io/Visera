module;
#include <Visera.h>
#include <tbb/tbb.h>
export module Visera.Runtime.Render.Camera;
export import Visera.Runtime.Render.Camera.Lens;
export import Visera.Runtime.Render.Camera.Film;

import Visera.Core.Math.Basic;
import Visera.Core.Media.Image;

import Visera.Runtime.World.Atlas;
import Visera.Runtime.Render.Scene;
import Visera.Runtime.Render.RTC;

export namespace VE
{
	
	class FCamera
	{
	public:
		enum class EMode
		{ Orthographic, Perspective , Default = Perspective };

		void Shoot(SharedPtr<const FScene> _Scene) const;
		auto GetFilm() -> SharedPtr<IFilm> { return Film; }

		void SetLens(SharedPtr<ILens> _NewLens) { Lens = std::move(_NewLens); }
		void SetFilm(SharedPtr<IFilm> _NewFilm) { Film = std::move(_NewFilm); }

		auto GetNear() const -> Float { return Near; }
		void SetNear(Float _NewNear)  { bUpdateProject = True; Clamp(&_NewNear, 0.01, Far); Near = _NewNear; }
		auto GetFar()  const -> Float { return Far; }
		void SetFar(Float _NewFar)    { bUpdateProject = True; Clamp(&_NewFar, Near, 10000); Far = _NewFar;  } //Max: 100 Meters
		auto GetFOV() const -> Degree { return FOV; }
		void SetFOV(Degree _NewFOV)   { bUpdateProject = True; Clamp(reinterpret_cast<Float*>(&_NewFOV), 10.0, 90.0); FOV = _NewFOV; }
		auto GetAspectRatio() const -> Float { return AspectRatio; }
		void SetAspectRatio(Float _NewAspectRatio) { bUpdateProject = True; AspectRatio = _NewAspectRatio; }

		auto GetPosition() const -> const Vector3F& { return Origin; }
		void SetPosition(const Vector3F& _NewPosition) { bUpdateViewing = True; Origin.x() = _NewPosition.x(); Origin.y() = _NewPosition.y(); Origin.z() = _NewPosition.z(); }

		auto GetViewingMatrix() const -> const Matrix4x4F&;
		auto GetProjectMatrix() const -> const Matrix4x4F&;

		FCamera(EMode _Mode = EMode::Default) : Mode {_Mode} {};

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Forward= Atlas::Visera.Forward;
		Float      Near   = 0.01;
		Float      Far    = 100.0;
		Degree     FOV    = 90.0;
		Float      AspectRatio = 16.0 / 9.0;

		mutable Matrix4x4F ViewingMatrix = Matrix4x4F::Identity();
		mutable Matrix4x4F ProjectMatrix = Matrix4x4F::Identity();

		SharedPtr<ILens>  Lens;
		SharedPtr<IFilm>  Film;

		mutable Bool bUpdateViewing = True;
		mutable Bool bUpdateProject = True;
	};

	const Matrix4x4F& FCamera::
	GetViewingMatrix() const
	{
		if (bUpdateViewing)
		{
			ViewingMatrix = Matrix4x4F::Identity();
			bUpdateViewing = False;
		}

		return ViewingMatrix;
	}

	const Matrix4x4F& FCamera::
	GetProjectMatrix() const
	{
		//Reversed Z
		if (bUpdateProject)
		{
			ProjectMatrix = Matrix4x4F::Zero();
			ProjectMatrix(0,0) = 1/(AspectRatio * Tan(FOV/2.0));
			ProjectMatrix(1,1) = 1.0 / Tan(FOV/2.0);
			ProjectMatrix(2,2) = Far/(Far - Near);
			ProjectMatrix(2,3) = Far * Near / (Near - Far);
			ProjectMatrix(3,2) = 1.0;

			bUpdateProject = False;
		}

		return ProjectMatrix;
	}

	void FCamera::
	Shoot(SharedPtr<const FScene> _Scene) const
	{
		//[FIXME]: Testing
		tbb::parallel_for(0, 1000000, [&](Int32 k)
		{
			float i = (2*(k / 1000) - 1000) * 0.001;
			float j = (2*(k % 1001) - 1000) * 0.001;

			//auto FocusPoint = Lens->Sample();
			RTC::FRay Ray{{Origin.x() + i, Origin.y() - j, Origin.z() },
			 		 {i, -j, -1}};
			//FRay Ray{{0, 0 , 2}, {i, j , -1}};

			_Scene->Accept(&Ray);
			if (Ray.HasHit())
			{
				auto Normal = Ray.GetHitInfo().GetSurfaceNormal();
				if (Ray.GetDirection().dot(Normal) > 0) { return; }

				auto NormalColor = (Normal * 0.5 + Vector3F{ 0.5, 0.5, 0.5 }) * 255;
				UInt32 X = (i + 1.0) * (Film->GetWidth()/2);
				UInt32 Y = (j + 1.0) * (Film->GetHeight()/2);

				FImage::FPixel Pixel{};
				Pixel.R = NormalColor.x();
				Pixel.G = NormalColor.y();
				Pixel.B = NormalColor.z();

				Film->Develop()->SetPixel(X, Y, Pixel);
			}
		});
	}

} // namespace VE