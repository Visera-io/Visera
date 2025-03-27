module;
#include <Visera.h>
#include <tbb/tbb.h>
export module Visera.Runtime.World.Camera;
export import Visera.Runtime.World.Camera.Lens;
export import Visera.Runtime.World.Camera.Film;

import Visera.Core.Math.Basic;
import Visera.Core.Media.Image;
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
		for (Float i = -1.0; i <= 1.0; i += 0.01)
		{
			for (Float j = -1.0; j <= 1.0; j += 0.01)
			{
				auto FocusPoint = Lens->Sample();
				FRay Ray{{Origin.x() + FocusPoint.x(), Origin.y() + FocusPoint.y(), Origin.z() },
						 {i, j, 1}};

				Ray.CastTo(_Scene);
				if (Ray.HasHit())
				{
					auto Normal = Ray.GetHitInfo().GetSurfaceNormal();
					if (Ray.GetDirection().dot(Normal) > 0) { return; }

					auto NormalColor = (Normal * 0.5 + Vector3F{ 0.5, 0.5, 0.5 }) * 255;
					UInt32 X = (i + 1.0) * (Film->GetWidth()/2);
					UInt32 Y = (j + 1.0) * (Film->GetHeight()/2);

					FImage::FPixel Pixel;
					Pixel.R = NormalColor.x();
					Pixel.G = NormalColor.y();
					Pixel.B = NormalColor.z();

					Film->Negative.SetPixel(X, Y, Pixel);
				}
			}
		}
	}

	VCamera::
	VCamera(SharedPtr<ILens> _Lens,
			SharedPtr<IFilm> _Film,
			EMode _Mode/* = EMode::Default */)
			:
			Lens{ std::move(_Lens) },
			Film{std::move(_Film)  },
			Mode {_Mode}
	{
		VE_ASSERT(Lens != nullptr && Film != nullptr);
	}

} // namespace VE