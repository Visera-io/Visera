module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Sampler;
#define VE_MODULE_NAME "Vulkan:Sampler"
import :Context;
import Visera.Core.Log;

import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :GPU;

export namespace VE
{

    class FVulkanSampler : public std::enable_shared_from_this<FVulkanSampler>
    {
    public:
        static inline auto
        Create(EVulkanFilter _Filter, EVulkanSamplerAddressMode _AddressMode = EVulkanSamplerAddressMode::ClampToEdge)
        { return CreateSharedPtr<FVulkanSampler>(_Filter, _AddressMode); }
        auto inline
        SetFilters(EVulkanFilter _ZoomIn, EVulkanFilter _ZoomOut) { Filters.ZoomIn = _ZoomIn; Filters.ZoomOut = _ZoomOut; return this; }
        auto inline
        Build() -> SharedPtr<FVulkanSampler>;
        void inline
        Destroy();

        Bool inline
        IsBuilt() const { return Handle != VK_NULL_HANDLE; }

        auto GetHandle() const -> const VkSampler { return Handle; }

        FVulkanSampler(EVulkanFilter _Filter,
                       EVulkanSamplerAddressMode _AddressMode = EVulkanSamplerAddressMode::ClampToEdge);
        ~FVulkanSampler() { Destroy(); }

    private:
        VkSampler Handle {VK_NULL_HANDLE};
        union
        {
            EVulkanSamplerAddressMode Data[3]
            {
                EVulkanSamplerAddressMode::ClampToEdge,
                EVulkanSamplerAddressMode::ClampToEdge,
                EVulkanSamplerAddressMode::ClampToEdge,
            };
            struct {EVulkanSamplerAddressMode U, V, W;};
        } AddressModes;

        union
        {
            EVulkanFilter Data[2]
            {
                EVulkanFilter::Linear,
                EVulkanFilter::Linear,
            };
            struct {EVulkanFilter ZoomIn, ZoomOut;};
        } Filters;

        EVulkanBorderColor		    BorderColor = EVulkanBorderColor::Black_Int;
        EVulkanSamplerMipmapMode    MipmapMode  = EVulkanSamplerMipmapMode::Linear;
        EVulkanCompareOp	        CompareMode = EVulkanCompareOp::Never;
        Bool                        bAnisotropy = True;
    };

    SharedPtr<FVulkanSampler> FVulkanSampler::
    Build()
    {
        VkSamplerCreateInfo CreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,

            .magFilter = AutoCast(Filters.ZoomIn),
            .minFilter = AutoCast(Filters.ZoomOut),

            .mipmapMode = AutoCast(MipmapMode),

            .addressModeU = AutoCast(AddressModes.U),
            .addressModeV = AutoCast(AddressModes.V)? AutoCast(AddressModes.V) : AutoCast(AddressModes.U),
            .addressModeW = AutoCast(AddressModes.W)? AutoCast(AddressModes.W) : AutoCast(AddressModes.U),

            .mipLodBias   = 0.0,

            .anisotropyEnable = bAnisotropy? VK_TRUE : VK_FALSE,
            .maxAnisotropy	  = GVulkan->GPU->GetProperties().limits.maxSamplerAnisotropy,

            .compareEnable	= AutoCast(CompareMode)? VK_TRUE : VK_FALSE,
            .compareOp		= AutoCast(CompareMode),

            .minLod = -1000.0,
            .maxLod = +1000.0,

            .borderColor                = AutoCast(BorderColor),
            .unnormalizedCoordinates    = VK_FALSE
        };

        if (vkCreateSampler(
            GVulkan->Device->GetHandle(),
            &CreateInfo,
            GVulkan->AllocationCallbacks,
            &Handle) != VK_SUCCESS)
        { VE_LOG_FATAL("Failed to create the Vulkan Sampler!"); }

        VE_LOG_DEBUG("Built a new sampler (handle:{}, filter:[ZoomIn{},ZoomOut{}], address mode:[U{},V{},W{}]).",
			(Address)(Handle),
            UInt32(Filters.ZoomIn), UInt32(Filters.ZoomOut),
            UInt32(AddressModes.U), UInt32(AddressModes.V),UInt32(AddressModes.W));
        return shared_from_this();
    }

    FVulkanSampler::
    FVulkanSampler(
        EVulkanFilter             _Filter      /* = EVulkanFilter::Linear*/,
        EVulkanSamplerAddressMode _AddressMode /* = EVulkanSamplerAddressMode::ClampToEdge*/)
        :
        Filters{ .ZoomIn{_Filter}, .ZoomOut{_Filter} },
        AddressModes{ .U{_AddressMode}, .V{_AddressMode}, .W{_AddressMode} }
    {

    }

    void FVulkanSampler::
    Destroy()
    {
        if (IsBuilt())
        {
            vkDestroySampler(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
            Handle = VK_NULL_HANDLE;
        }
    }

} // namespace VE