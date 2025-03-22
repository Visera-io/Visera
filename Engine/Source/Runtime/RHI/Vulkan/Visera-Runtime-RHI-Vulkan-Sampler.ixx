module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Sampler;
import :Context;
import Visera.Core.Signal;

import :Common;
import :Device;
import :GPU;

export namespace VE
{

    class FVulkanSampler
    {
    public:
        auto GetHandle() const -> const VkSampler { return Handle; }

        FVulkanSampler();
        ~FVulkanSampler();

    private:
        VkSampler Handle {VK_NULL_HANDLE};
        union
        {
            EVulkanSamplerAddressMode Modes[3]
            {
                EVulkanSamplerAddressMode::Repeat,
                EVulkanSamplerAddressMode::Repeat,
                EVulkanSamplerAddressMode::Repeat,
            };
            struct {EVulkanSamplerAddressMode U, V, W;};
        } AddressModes;

        union
        {
            EVulkanFilter Filters[2]
            {
                EVulkanFilter::Nearest,
                EVulkanFilter::Nearest,
            };
            struct {EVulkanFilter ZoomIn, ZoomOut;};
        } Filters;

        EVulkanBorderColor		    BorderColor = EVulkanBorderColor::Black_Int;
        EVulkanSamplerMipmapMode    MipmapMode  = EVulkanSamplerMipmapMode::Linear;
        EVulkanCompareOp	        CompareMode = EVulkanCompareOp::Never;
        VkBool32                    bAnisotropy = VK_TRUE;
    };

    FVulkanSampler::
    FVulkanSampler()
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

            .anisotropyEnable = bAnisotropy,
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
        { throw SRuntimeError("Failed to create the Vulkan Sampler!"); }
    }

    FVulkanSampler::
    ~FVulkanSampler()
    {
        vkDestroySampler(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
        Handle = VK_NULL_HANDLE;
    }

} // namespace VE