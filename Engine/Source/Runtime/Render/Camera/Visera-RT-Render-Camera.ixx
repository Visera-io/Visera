module;
#include <Visera.h>
#include <tbb/tbb.h>
export module Visera.Runtime.Render.Camera;
export import Visera.Runtime.Render.Camera.Lens;
export import Visera.Runtime.Render.Camera.Film;

import Visera.Core.Math.Basic;
import Visera.Core.Media.Image;
import Visera.Core.Signal;

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
		void SetFOV(Degree _NewFOV)   { bUpdateProject = True; Mode == EMode::Perspective? FOV = GetClamped(_NewFOV.Data(), 10.0, 90.0) : FOV = _NewFOV; }
		auto GetAspectRatio() const -> Float { return AspectRatio; }
		void SetAspectRatio(Float _NewAspectRatio) { bUpdateProject = True; AspectRatio = _NewAspectRatio; }
		void SetProjectionMode(EMode _NewMode) { bUpdateProject = True; Mode = _NewMode; }

		auto GetForward() const -> const Vector3F& { return Forward; }
		void SetForward(const Vector3F& _NewForward) { bUpdateViewing = True; Forward = _NewForward; }
		auto GetPosition() const -> const Vector3F& { return Origin; }
		void SetPosition(const Vector3F& _NewPosition) { bUpdateViewing = True; Origin.x() = _NewPosition.x(); Origin.y() = _NewPosition.y(); Origin.z() = _NewPosition.z(); }

		auto GetViewingMatrix() const -> const Matrix4x4F&;
		auto GetProjectMatrix() const -> const Matrix4x4F&;
		auto GetLookAtMatrix(const Vector3F& _Target)  const -> Matrix4x4F;

		FCamera(EMode _Mode = EMode::Default) : Mode {_Mode} {};

	private:
		EMode      Mode   = EMode::Default;
		Vector3F   Origin = Atlas::Visera.Origin;
		Vector3F   Upward = Atlas::Visera.Upward;
		Vector3F   Right  = Atlas::Visera.Right;
		Vector3F   Forward= Atlas::Visera.Forward;
		Float      Near   = 0.1;
		Float      Far    = 100.0;
		Float      AspectRatio = 16.0 / 9.0;
		union
		{
			Degree FOV = 90.0; Float OrthoScale;
		};

		mutable Matrix4x4F ViewingMatrix = Matrix4x4F::Identity();
		mutable Matrix4x4F ProjectMatrix = Matrix4x4F::Identity();

		SharedPtr<ILens>  Lens;
		SharedPtr<IFilm>  Film;

		mutable Bool bUpdateViewing = True;
		mutable Bool bUpdateProject = True;
	};

	Matrix4x4F FCamera::
	GetLookAtMatrix(const Vector3F& _Target)  const
	{
		Vector3F ViewDir   = (_Target - Origin).normalized();
		Vector3F RightDir  = Upward.cross(ViewDir).normalized();
		Vector3F UpwardDir = RightDir.cross(ViewDir).normalized();

		Matrix4x4F LookAtMatrix = Matrix4x4F::Identity();
		LookAtMatrix.block<3,1>(0,0) = RightDir;
		LookAtMatrix.block<3,1>(0,1) = UpwardDir;
		LookAtMatrix.block<3,1>(0,2) = ViewDir;

		LookAtMatrix.block<3,1>(0,3) = Vector3F
		{-RightDir.dot(Origin), -UpwardDir.dot(Origin), -ViewDir.dot(Origin)};

		return LookAtMatrix;
	}

	const Matrix4x4F& FCamera::
	GetViewingMatrix() const
	{
		if (bUpdateViewing)
		{
			ViewingMatrix = Matrix4x4F::Identity();
			VE_WIP;
			bUpdateViewing = False;
		}

		return ViewingMatrix;
	}

	const Matrix4x4F& FCamera::
	GetProjectMatrix() const
	{
		if (!bUpdateProject) { return ProjectMatrix; }

		switch (Mode)
		{
		case EMode::Perspective:
		{
			//Reversed Z
			const Float TanHalfFOV = Tan(FOV/2.0);

			ProjectMatrix = Matrix4x4F::Zero();
			ProjectMatrix(0,0) = 1.0 / (AspectRatio * TanHalfFOV);
			ProjectMatrix(1,1) = 1.0 / TanHalfFOV;
			ProjectMatrix(2,2) = -Near / (Far - Near);
			ProjectMatrix(2,3) = Far * Near / (-Near + Far);
			ProjectMatrix(3,2) = 1.0;
			break;
		}
		case EMode::Orthographic:
		{
			//Reversed Z
			const Float Width  = OrthoScale;
			const Float Height = Width / AspectRatio;

			ProjectMatrix = Matrix4x4F::Identity();
			ProjectMatrix(0,0) = 1.0 / Width;
			ProjectMatrix(1,1) = 1.0 / Height;
			ProjectMatrix(2,2) = -1.0 / (Far - Near);
			ProjectMatrix(2,3) = Far / (Far - Near);
			break;
		}
		default: throw SRuntimeError("Unknown Camera mode!");
		}
		bUpdateProject = False;
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
			 		 {i, -j, Forward.z()}};
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