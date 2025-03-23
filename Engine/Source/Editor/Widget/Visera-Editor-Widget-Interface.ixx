module;
#include <Visera.h>
export module Visera.Editor.Widget:Interface;

export namespace VE
{

    class IWidget
    {
    public:
        virtual void
        Render() const = 0;
        Bool inline
        IsVisible() const { return bVisible; }

        virtual
        ~IWidget() = default;

    private:
        Bool bVisible = True;
    };

} // namespace VE