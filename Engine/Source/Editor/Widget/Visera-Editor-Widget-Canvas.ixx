module;
#include <Visera.h>
#include <imgui.h>
export module Visera.Editor.Widget:Canvas;

import :Interface;

export namespace VE
{

  class FCanvas : public IWidget
  {
  public:
      virtual void Render() const override
      { if (IsVisible()) { ImGui::Begin("Canvas"); ImGui::End(); } }
  };

} // namespace VE