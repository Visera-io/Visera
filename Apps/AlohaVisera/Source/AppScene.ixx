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

        UniquePtr<FLight> MainLight;

        FAppScene(SharedPtr<const RHI::FDescriptorSetLayout> _DSLayout)
        {
            MainCamera = CreateUniquePtr<FCamera>();
            MainCamera->SetFOV(90.0);
            MainCamera->SetPosition({ 0, 2, -4 });

            ShadowCamera = CreateUniquePtr<FCamera>(FCamera::EMode::Orthographic);
            ShadowCamera->SetFOV(4.0);
            ShadowCamera->SetPosition({2, 7, -6});

            MainLight = CreateUniquePtr<FLight>();
        }
        FAppScene() = delete;
    };
}