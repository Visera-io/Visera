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
		auto GetFilm() -> SharedPtr<IFilm> { return Film; }

		void SetLens(SharedPtr<ILens> _NewLens) { Lens = std::move(_NewLens); }
		void SetFilm(SharedPtr<IFilm> _NewFilm) { Film = std::move(_NewFilm); }

		auto GetPosition() const -> const Vector3F& { return Origin; }
		void SetPosition(const Vector3F& _NewPosition) { Origin.x() = _NewPosition.x(); Origin.y() = _NewPosition.y(); Origin.z() = _NewPosition.z(); }

		auto GetViewingMatrix() const -> const Matrix4x4F& { VE_WIP; return ViewingMatrix; }
		auto GetProjectMatrix() const -> const Matrix4x4F& { VE_WIP; return ProjectMatrix; }

		VCamera(EMode _Mode = EMode::Default) : Mode {_Mode} {};

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Forward= Atlas::Visera.Forward;

		Matrix4x4F ViewingMatrix;
		Matrix4x4F ProjectMatrix;

		SharedPtr<ILens>  Lens;
		SharedPtr<IFilm>  Film;

		Bool bUpdated = False;
	};

	void VCamera::
	Shoot(SharedPtr<const FScene> _Scene)
	{
		//[FIXME]: Testing
		tbb::parallel_for(0, 1000000, [&](Int32 k)
		{
			float i = (2*(k / 1000) - 1000) * 0.001;
			float j = (2*(k % 1001) - 1000) * 0.001;

			//auto FocusPoint = Lens->Sample();
			FRay Ray{{Origin.x() + i, Origin.y() + j, Origin.z() },
			 		 {i, j, -1}};
			//FRay Ray{{0, 0 , 2}, {i, j , -1}};

			Ray.CastTo(_Scene);
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