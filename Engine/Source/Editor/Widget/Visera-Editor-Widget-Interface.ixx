module;
#include <Visera.h>
export module Visera.Editor.Widget:Interface;

import Visera.Core.Type;

export namespace VE
{

    class IWidget
    {
    public:
        virtual void
        Render() const = 0;
        Bool inline
        IsVisible() const { return bVisible; }
        auto inline
        GetName() const -> const FName& { return Name; }

        void inline
        Hide()   const { bVisible = False; }
        void inline
        Reveal() const { bVisible = True; }

        IWidget() = delete;
        IWidget(const FName& _Name) : Name(_Name) {}
        virtual
        ~IWidget() = default;

    private:
        FName   Name;
        mutable Bool bVisible = True;
    };

} // namespace VE