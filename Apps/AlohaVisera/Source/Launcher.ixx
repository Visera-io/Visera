module;
#include <Visera.h>
#include <imgui.h>
export module AlohaVisera;
#define VE_MODULE_NAME "AlohaVisera"

import Visera;
using namespace VE;

export namespace VISERA_APP_NAMESPACE
{
	
	class App final : public ViseraApp
	{
		UniquePtr<FCamera>  Camera;
		SharedPtr<FScene>   Scene = FScene::Create();
		WeakPtr<Editor::FCanvas>  SampleCanvas;
		WeakPtr<Editor::FCanvas>  TextureCanvas;
		SharedPtr<IImage> Image;
		Bool bCameraMoved = False;
        
		SharedPtr<FURPBackgroundPass> TestRenderPass;
		SharedPtr<FURPGeometryPass> GeometryPass;
		SharedPtr<FURPPostprocessPass> PostprocessingPass;
		const FScene::FAttachment* GeoAttachment = nullptr;
		const FScene::FAttachment* CubeAttachment = nullptr;
		RHI::FMatrixUBOLayout MatrixUBOData;

		WeakPtr<Editor::FCanvas> TestCanvas;

		Matrix4x4F LookAtMatrix;
		Vector3F Target;
	public:
		virtual void Tick() override
		{
			auto& Frame = RHI::GetCurrentFrame();
			auto GCmds = Frame.GetGraphicsCommandBuffer();
			GCmds->ReachRenderPass(TestRenderPass);
			{
				GCmds->BindRenderPipeline(TestRenderPass->GetSkyboxPipeline());
				GCmds->Draw(3);
			}
			GCmds->LeaveRenderPass(TestRenderPass);

			GCmds->ReachRenderPass(GeometryPass);
			{
				GCmds->BindRenderPipeline(GeometryPass->GetOpaquePipeline());

				static Vector3F Scales{1.0, 1.0, 1.0};
				//static Float Scale = 1.0;//0.8 + 0.5 * Sin(Radian{0.005f * OS::GetRunningTime().milliseconds()});
				static Vector3F Translate{0, 0, 0.2 };
				if (ImGui::InputFloat3("Scales", Scales.data()))
				{
					MatrixUBOData.Model(0,0) = Scales.x();
					MatrixUBOData.Model(1,1) = Scales.y();
					MatrixUBOData.Model(2,2) = Scales.z();
				}

				if (ImGui::InputFloat3("Translate", Translate.data()))
				{
					MatrixUBOData.Model(0,3) = Translate.x();
					MatrixUBOData.Model(1,3) = Translate.y();
					MatrixUBOData.Model(2,3) = Translate.z();
				}

				Frame.SetModelMatrix(MatrixUBOData.Model);
				static Matrix4x4F CofactorMat = Matrix4x4F::Identity();
				CofactorMat.block<3,3>(0,0) = ComputeCofactorMatrixFromHomogeneous(MatrixUBOData.Model);
				Frame.SetCofactorModelMatrix(CofactorMat);

				static Vector3F CamPos = Camera->GetPosition();
				static Float Fov{90};
				if (ImGui::InputFloat("FOV", &Fov))
				{
					Camera->SetFOV(Degree{Fov});
				}

				static Bool bPerspective = True;
				if (ImGui::Button("Camera Mode"))
				{
					bPerspective = !bPerspective;
					if (bPerspective) Camera->SetProjectionMode(FCamera::EMode::Perspective);
					else Camera->SetProjectionMode(FCamera::EMode::Orthographic);
				}
				Target= GeoAttachment->GetPrimitive()->GetBoundingBox().Center;
				LookAtMatrix = Camera->GetLookAtMatrix(Target);
				if(ImGui::InputFloat3("CamPos", CamPos.data()))
				{
					Camera->SetPosition(CamPos);
					LookAtMatrix = Camera->GetLookAtMatrix(Target);
				}
				Frame.SetViewingMatrix(LookAtMatrix);
				//Frame.SetViewingMatrix(Camera->GetViewingMatrix());
				Frame.SetProjectionMatrix(Camera->GetProjectMatrix());

				GCmds->BindDescriptorSet(0, Frame.GetMatrixUBO());
				GCmds->BindDescriptorSet(1, TestCanvas.lock()->GetTexture());

				GCmds->BindVertexBuffer(GeoAttachment->GetPrimitive()->GetGPUVertexBuffer());

				if (GeoAttachment->GetPrimitive()->HasIndex())
				{
					GCmds->BindIndexBuffer(GeoAttachment->GetPrimitive()->GetGPUIndexBuffer());
					GCmds->DrawIndexed(GeoAttachment->GetPrimitive()->GetIndexCount());
				}
				else
				{
					GCmds->Draw(3 * GeoAttachment->GetPrimitive()->GetVertexCount());
				}
			}
			GCmds->LeaveRenderPass(GeometryPass);

			GCmds->ReachRenderPass(GeometryPass);
			{
				GCmds->BindRenderPipeline(GeometryPass->GetOpaquePipeline());

				GCmds->BindDescriptorSet(0, Frame.GetMatrixUBO());
				GCmds->BindDescriptorSet(1, TextureCanvas.lock()->GetTexture());

				GCmds->BindVertexBuffer(CubeAttachment->GetPrimitive()->GetGPUVertexBuffer());

				GCmds->BindIndexBuffer(CubeAttachment->GetPrimitive()->GetGPUIndexBuffer());
				GCmds->DrawIndexed(CubeAttachment->GetPrimitive()->GetIndexCount());
			}
			GCmds->LeaveRenderPass(GeometryPass);

			GCmds->ReachRenderPass(PostprocessingPass);
			{
				GCmds->BindRenderPipeline(PostprocessingPass->GetGammaCorrectionPipeline());
				GCmds->BindDescriptorSet(0, Frame.GetSVColorTexture());
				GCmds->Draw(3);
			}
			GCmds->LeaveRenderPass(PostprocessingPass);

			ImGui::Text("FPS: %d", RHI::GetFPS());

			//Exit();
		}

		virtual void Bootstrap() override
		{
			//VE_LOG_INFO(Bootstraping " VISERA_APP_NAME);
			auto Config = FileSystem::CreateJSONFile(FPath{"D:/Programs/ViseraEngine/Visera/Engine/Configs/Configs.json"});
			Config->Load();
			auto JSON = Config->Parse();
			VE_LOG_INFO("{}", JSON["Engine"]["Assets"]["Models"]["test"].GetString());
			auto Model = Media::CreateModel(FName{ "model" },
				FPath{"D:/Programs/ViseraEngine/Visera/Engine/Assets/Models/"}
				.Join(FPath{JSON["Engine"]["Assets"]["Models"]["test"].GetString()}));

			auto CubePlane = Media::CreateModel(FName{ "model_cube_0" },
				FPath{"D:/Programs/ViseraEngine/Visera/Engine/Assets/Models/"}
				.Join(FPath{JSON["Engine"]["Assets"]["Models"]["box"].GetString()}));

			GeoAttachment = &Scene->Attach(FName{"model_0"}, Model);
			CubeAttachment = &Scene->Attach(FName{"model_1"}, CubePlane);
			Scene->Commit();
			auto LogoImage = Media::CreateImage(FName{"logo"}, FPath{"D:/Programs/ViseraEngine/Visera/Engine/Assets/Images/Logo.png"});
			LogoImage->FlipVertically();
			Editor::CreateCanvas(LogoImage);
			VE_LOG_INFO("\nMinBB:{}\nMaxBB:{}\nCenter:{}",
				Text(GeoAttachment->GetPrimitive()->GetBoundingBox().MinPosition),
				Text(GeoAttachment->GetPrimitive()->GetBoundingBox().MaxPosition),
				Text(GeoAttachment->GetPrimitive()->GetBoundingBox().Center));

			VE_LOG_INFO("\nMinBB:{}\nMaxBB:{}\nCenter:{}",
				Text(CubeAttachment->GetPrimitive()->GetBoundingBox().MinPosition),
				Text(CubeAttachment->GetPrimitive()->GetBoundingBox().MaxPosition),
				Text(CubeAttachment->GetPrimitive()->GetBoundingBox().Center));

			//QuadImage = Media::CreateImage(FName{"quad_image"}, 400, 400);
			Image = Media::CreateImage(FName{"image"}, FPath{"D:/Programs/ViseraEngine/Visera/Engine/Assets/Models/Marry/MC003_Kozakura_Mari.png"});
			///SampleCanvas = Editor::CreateCanvas(QuadImage);
			TextureCanvas = Editor::CreateCanvas(Media::CreateImage(FName{"default_texture"},
				FPath{"D:/Programs/ViseraEngine/Visera/Engine/Assets/Images/"}
				.Join(FPath{JSON["Engine"]["Assets"]["Images"]["default_texture"].GetString()})));

		    Camera = CreateUniquePtr<FCamera>();
			Camera->SetPosition({0, 3, -4});
			Camera->SetLens(CreateSharedPtr<FPinhole>());
			Camera->SetFilm(CreateSharedPtr<FRawFilm>(400, 400));

			TestCanvas = Editor::CreateCanvas(Image);

			TestRenderPass = RHI::CreateRenderPass<FURPBackgroundPass>();
			GeometryPass   = RHI::CreateRenderPass<FURPGeometryPass>();
			PostprocessingPass = RHI::CreateRenderPass<FURPPostprocessPass>();

			IO::RegisterMouseButtonEvent(IO::FMouseButtonCreateInfo
				{
					.Name   = FName{"camera_zoomin"},
					.Button	= IO::EMouseButton::Left,
					.Action = IO::EAction::Press,
					.Event  = [&]()
					{
						auto CurrentPos = Camera->GetPosition();
						Camera->SetPosition(Vector3F{CurrentPos.x(), CurrentPos.y(), CurrentPos.z() + 0.2f});
						LookAtMatrix = Camera->GetLookAtMatrix(Target);
					},
				});
			IO::RegisterMouseButtonEvent(IO::FMouseButtonCreateInfo
			{
				.Name   = FName{"camera_zoomout"},
				.Button	= IO::EMouseButton::Right,
				.Action = IO::EAction::Press,
				.Event  = [&]()
				{
					auto CurrentPos = Camera->GetPosition();
					Camera->SetPosition(Vector3F{CurrentPos.x(), CurrentPos.y(), CurrentPos.z() - 0.2f});
					LookAtMatrix = Camera->GetLookAtMatrix(Target);
				},
			});
			IO::RegisterKeyboardKeyEvent(IO::FKeyboardKeyEventCreateInfo
				{
					.Name   = FName{"pause"},
					.Key	= IO::EKey::Space,
					.Action = IO::EAction::Detach,
					.Event = []() {
						Log::Warn("System Paused! (Input any key in CMD to continue)");
						String i;
						std::cin >> i;
					},
				});
		}

		virtual void Terminate() override
		{
			//Film->SaveAs(FPath{ "D:\\Downloads\\Film.bmp" });
		}
	};
} // namespace VISERA_APP_NAMESPACE


export int main(int argc, char* argv[])
{
	return VE::ViseraEngine::Run(new VISERA_APP_NAMESPACE::App);
}