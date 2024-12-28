module;
#include <Visera.h>

#include <Windows.h>
#include <directx-dxc/dxcapi.h>

export module Visera.Runtime.Render.DXC;

export namespace VE
{
    inline IDxcUtils* pLib{ nullptr };

} // namespace VE