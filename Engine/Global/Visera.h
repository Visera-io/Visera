#pragma once
// << Layers >>
#define VISERA_RUNTIME

#include "PCH/Visera-STD.h"
#include "PCH/Visera-Types.h"

// << Global Macros >>
#define VISERA_ENGINE_ERROR -100
#define VISERA_APP_ERROR    -200

#define VISERA_PUBLIC_MODULE(ModuleName)    export namespace ModuleName {
#define VISERA_PRIVATE_MODULE(ModuleName)   namespace ModuleName {
#define VISERA_MODULE_END(ModuleName)       "} // namespace " #ModuleName

#define VE_ASSERT(Expression)				assert(Expression);

namespace VE
{

}