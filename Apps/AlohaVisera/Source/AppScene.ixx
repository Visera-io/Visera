module;
#include <Visera.h>
export module AlohaVisera:AppScene;
#define VE_MODULE_NAME "AlohaVisera:AppScene"

import Visera;
using namespace VE;

export namespace VISERA_APP_NAMESPACE
{

    struct FAppScene
    {
        static inline Vector3F LookAtTarget;

        UniquePtr<FCamera> MainCamera;
        UniquePtr<FCamera> ShadowCamera;

        FURPShadowPass::FShadowPassShaderUBO LightUBOData;
        SharedPtr<RHI::FDescriptorSet> LightUBO;
        SharedPtr<RHI::FBuffer>        LightUBOBuffer;

        FAppScene(SharedPtr<const RHI::FDescriptorSetLayout> _DSLayout)
        {
            MainCamera = CreateUniquePtr<FCamera>();
            MainCamera->SetFOV(90.0);
            MainCamera->SetPosition({ 0, 2, -4 });

            ShadowCamera = CreateUniquePtr<FCamera>(FCamera::EMode::Orthographic);
            ShadowCamera->SetFOV(4.0);
            ShadowCamera->SetPosition({2, 7, -6});

            LightUBOBuffer = RHI::CreateMappedBuffer(sizeof LightUBOData, RHI::EBufferUsage::Uniform);

            LightUBOData.MVPMatrix = (
                    ShadowCamera->GetProjectMatrix() *
                    ShadowCamera->GetLookAtMatrix(LookAtTarget)).eval();

            LightUBOBuffer->Write(LightUBOData.MVPMatrix.data(), sizeof LightUBOData.MVPMatrix);

            LightUBO = RHI::CreateDescriptorSet(_DSLayout);
            LightUBO->WriteBuffer(0, LightUBOBuffer, 0, LightUBOBuffer->GetSize());
        }
        FAppScene() = delete;
    };
}