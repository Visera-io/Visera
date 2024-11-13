module;
#include <Visera>

#include <Windows.h>
#include <directx-dxc/dxcapi.h>

export module Visera.Engine.Runtime.Render.DXC;

export namespace VE { namespace Runtime
{
    inline IDxcUtils* pLib{ nullptr };

} } // namespace VE::Runtime