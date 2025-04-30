module;
#include <Visera.h>
export module Visera.Runtime.Render.Texture:Interface;
#define VE_MODULE_NAME "Texture:Interface"
import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class ITexture : public std::enable_shared_from_this<ITexture>
    {
    public:
        auto inline
        GetWidth() const { return Extent.width; }
        auto inline
        GetHeight() const { return Extent.height; }

        void inline
        BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const;

        ITexture() = default;
        ITexture(UInt32 _Width, UInt32 _Height, UInt32 _Depth = 1) : Extent{_Width, _Height, _Depth} { VE_ASSERT(_Width > 0 && _Height > 0 && _Depth > 0); };
        virtual ~ITexture() = default;

    protected:
        RHI::FExtent3D                 Extent;

        SharedPtr<RHI::FImage>         RHIImage;
        SharedPtr<RHI::FImageView>     RHIImageView;
        SharedPtr<const RHI::FSampler> Sampler;

        struct FSupportedFormat
        {
            const char*   Name;
            RHI::EFormat  Format;
            RHI::FSwizzle Swizzle{};
        };
    };

    void ITexture::
    BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const
    {
        VE_ASSERT(!_DescriptorSet->IsExpired());
        VE_LOG_DEBUG("Binding ShadowMap ({}) to set({}:{})",
            (Address)(RHIImage->GetHandle()),
            (Address)(_DescriptorSet->GetHandle()), _Binding);
        _DescriptorSet->WriteImage(_Binding, RHIImageView, Sampler);
    }

} // namespace VE