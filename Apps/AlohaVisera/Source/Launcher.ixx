module;
#include <Visera.h>
#include <imgui.h>
export module AlohaVisera;
#define VE_MODULE_NAME "AlohaVisera"
import :AppScene;

import Visera;
using namespace VE;

export namespace VISERA_APP_NAMESPACE
{
	
	class App final : public ViseraApp
	{
		UniquePtr<FAppScene> AppScene;

		SharedPtr<FScene>   Scene = FScene::Create();
		WeakPtr<Editor::FCanvas>  SampleCanvas;
		WeakPtr<Editor::FCanvas>  TextureCanvas;
		SharedPtr<IImage> MariTexImage;

		SharedPtr<FURPBackgroundPass>  BackgroundPass;
		SharedPtr<FURPShadowPass>      ShadowPass;
		SharedPtr<FURPGeometryPass>    GeometryPass;
		SharedPtr<FURPPostprocessPass> PostprocessingPass;

		const FScene::FAttachment* GeoAttachment  = nullptr;
		const FScene::FAttachment* CubeAttachment = nullptr;
		RHI::FMatrixUBOLayout MatrixUBOData;

		WeakPtr<Editor::FCanvas> MariTexCanvas;

		Matrix4x4F LookAtMatrix;
		Vector3F Target;

	public:
		virtual void Tick() override
		{
			auto& Frame = RHI::GetCurrentFrame();
			auto GCmds = Frame.GetGraphicsCommandBuffer();
			GCmds->ReachRenderPass(BackgroundPass);
			{
				GCmds->BindRenderPipeline(BackgroundPass->GetSkyboxPipeline());
				GCmds->Draw(3);
			}
			GCmds->LeaveRenderPass(BackgroundPass);

			GCmds->ReachRenderPass(ShadowPass);
			{
				GCmds->BindRenderPipeline(ShadowPass->GetParallelShadowPipeline());

				Target = GeoAttachment->GetPrimitive()->GetBoundingBox().Center;
				LookAtMatrix = AppScene->ShadowCamera->GetLookAtMatrix(Target);

				Frame.SetLightSpaceTransform(AppScene->ShadowCamera->GetProjectMatrix() * LookAtMatrix);

				GCmds->BindDescriptorSet(0, Frame.GetLightUBO());
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

				//GCmds->BindVertexBuffer(CubeAttachment->GetPrimitive()->GetGPUVertexBuffer());
				//GCmds->BindIndexBuffer(CubeAttachment->GetPrimitive()->GetGPUIndexBuffer());
				//GCmds->DrawIndexed(CubeAttachment->GetPrimitive()->GetIndexCount());
			}
			GCmds->LeaveRenderPass(ShadowPass);

			GCmds->ConvertImageLayout(Frame.GetSVShadowImage(), RHI::EImageLayout::ShaderReadOnly);
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

				static Vector3F CamPos = Vector3F{ AppScene->MainCamera->GetPosition() };
				static Float Fov = AppScene->MainCamera->GetFOV();
				if (ImGui::InputFloat("FOV", &Fov))
				{ AppScene->MainCamera->SetFOV(Degree{Fov}); }

				static Bool bPerspective = True;
				if (ImGui::Button("Camera Mode"))
				{
					bPerspective = !bPerspective;
					if (bPerspective) AppScene->MainCamera->SetProjectionMode(FCamera::EMode::Perspective);
					else AppScene->MainCamera->SetProjectionMode(FCamera::EMode::Orthographic);
				}
				Target= GeoAttachment->GetPrimitive()->GetBoundingBox().Center;
				LookAtMatrix = AppScene->MainCamera->GetLookAtMatrix(Target);
				if(ImGui::InputFloat3("CamPos", CamPos.data()))
				{
					AppScene->MainCamera->SetPosition(CamPos);
					LookAtMatrix = AppScene->MainCamera->GetLookAtMatrix(Target);
				}
				Frame.SetViewingMatrix(LookAtMatrix);
				//Frame.SetViewingMatrix(AppScene->MainCamera->GetViewingMatrix());
				Frame.SetProjectionMatrix(AppScene->MainCamera->GetProjectMatrix());

				GCmds->BindDescriptorSet(0, Frame.GetMatrixUBO());
				GCmds->BindDescriptorSet(1, Frame.GetLightUBO());
				GCmds->BindDescriptorSet(2, MariTexCanvas.lock()->GetTexture());
				GCmds->BindDescriptorSet(3, Frame.GetSVShadowMap());

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
				GCmds->BindDescriptorSet(1, Frame.GetLightUBO());
				GCmds->BindDescriptorSet(2, TextureCanvas.lock()->GetTexture());
				GCmds->BindDescriptorSet(3, Frame.GetSVShadowMap());

				GCmds->BindVertexBuffer(CubeAttachment->GetPrimitive()->GetGPUVertexBuffer());

				GCmds->BindIndexBuffer(CubeAttachment->GetPrimitive()->GetGPUIndexBuffer());
				GCmds->DrawIndexed(CubeAttachment->GetPrimitive()->GetIndexCount());
			}
			GCmds->LeaveRenderPass(GeometryPass);

			GCmds->ConvertImageLayout(Frame.GetSVShadowImage(), RHI::EImageLayout::DepthAttachment);

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
			auto K = Media::CreateImage(FName{"skybox"},
				GEngine::Path.Assets.Join(FPath{"Images/HDR/autumn_field_puresky_4k.exr"}));

			//VE_LOG_INFO(Bootstraping " VISERA_APP_NAME);
			auto Model = Media::CreateModel(FName{ "model" },
				GEngine::Path.Assets
				.Join(FPath{GEngine::GetConfigs()["Assets"]["Models"]["test"].GetString()}));

			auto CubePlane = Media::CreateModel(FName{ "model_cube_0" },
				GEngine::Path.Assets.Join(FPath{"Models/box.obj"}));

			GeoAttachment = &Scene->Attach(FName{"model_0"}, Model);
			CubeAttachment = &Scene->Attach(FName{"model_1"}, CubePlane, FScene::FlipFaceWinding);
			Scene->Commit();

			auto LogoImage = Media::CreateImage(FName{"logo"},
			GEngine::Path.Assets.Join(FPath{ "Images/Logo.png" }));
			LogoImage->FlipVertically();
			Editor::CreateCanvas(LogoImage);

			MariTexImage = Media::CreateImage(FName{"MariTexImage"},
			GEngine::Path.Assets.Join(FPath{"Models/Marry/MC003_Kozakura_Mari.png"}));

			TextureCanvas = Editor::CreateCanvas(Media::CreateImage(FName{"default_texture"},
				GEngine::Path.Assets.Join(FPath{"Images/default_texture.png"})));
			TextureCanvas.lock()->Hide();	

			MariTexCanvas = Editor::CreateCanvas(MariTexImage);
			
			VE_LOG_DEBUG("Creating Render Passes...");
			BackgroundPass = RHI::CreateRenderPass<FURPBackgroundPass>();
			ShadowPass     = RHI::CreateRenderPass<FURPShadowPass>();
			GeometryPass   = RHI::CreateRenderPass<FURPGeometryPass>();
			PostprocessingPass = RHI::CreateRenderPass<FURPPostprocessPass>();

			
			FAppScene::LookAtTarget = GeoAttachment->GetPrimitive()->GetBoundingBox().Center;
			AppScene = CreateUniquePtr<FAppScene>(ShadowPass->GetUBOLayout());


			IO::RegisterMouseButtonEvent(IO::FMouseButtonCreateInfo
				{
					.Name   = FName{"main_camera_zoomin"},
					.Button	= IO::EMouseButton::Left,
					.Action = IO::EAction::Press,
					.Event  = [&]()
					{
						auto CurrentPos = AppScene->MainCamera->GetPosition();
						AppScene->MainCamera->SetPosition(Vector3F{CurrentPos.x(), CurrentPos.y(), CurrentPos.z() + 0.2f});
						LookAtMatrix = AppScene->MainCamera->GetLookAtMatrix(Target);
					},
				});
			IO::RegisterMouseButtonEvent(IO::FMouseButtonCreateInfo
			{
				.Name   = FName{"main_camera_zoomout"},
				.Button	= IO::EMouseButton::Right,
				.Action = IO::EAction::Press,
				.Event  = [&]()
				{
					auto CurrentPos = AppScene->MainCamera->GetPosition();
					AppScene->MainCamera->SetPosition(Vector3F{CurrentPos.x(), CurrentPos.y(), CurrentPos.z() - 0.2f});
					LookAtMatrix = AppScene->MainCamera->GetLookAtMatrix(Target);
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
			
		}
	};
} // namespace VISERA_APP_NAMESPACE


export int main(int argc, char* argv[])
{
	return VE::ViseraEngine::Run(new VISERA_APP_NAMESPACE::App);
}