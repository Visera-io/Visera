module;
#include <Visera.h>
export module Visera.Runtime.Render.RHI;
#define VE_MODULE_NAME "RHI"

import Visera.Runtime.Render.RHI.Core;

export namespace VE
{

    class RHI
    {
        VE_MODULE_MANAGER_CLASS(RHI)
    public:
        static inline void
        Bootstrap();
        static inline void
        Terminate();

    private:
        static inline SharedPtr<FLoader>   Loader;
        static inline SharedPtr<FInstance> Instance;

    public:
		using ESharingMode			= EVkSharingMode;
		using ESampleRate			= EVkSampleRate;
		using EQueueFamily			= EVkQueueFamily;
		using ECommandPoolType		= EVkCommandPoolType;
		using ECommandLevel			= EVkCommandLevel;
		using EShaderStage			= EVkShaderStage;
		using EAccess				= EVkAccess;
		using EGraphicsPipelineStage= EVkGraphicsPipelineStage;
		using EComputePipelineStage = EVkComputePipelineStage;
		using ETransferPipelineStage= EVkTransferPipelineStage;
		using EMemoryUsage			= EVkMemoryUsage;
		using EBufferUsage			= EVkBufferUsage;
		using EAttachmentIO			= EVkAttachmentIO;
		using EDescriptorType		= EVkDescriptorType;
		using EImageType			= EVkImageType;
		using EImageLayout			= EVkImageLayout;
		using EImageUsage			= EVkImageUsage;
		using EImageViewType		= EVkImageViewType;
		using EImageAspect			= EVkImageAspect;
		using EImageTiling			= EVkImageTiling;
		using EFormat				= EVkFormat;
		using EFilter				= EVkFilter;
		using EPresentMode			= EVkPresentMode;
		using EBorderColor			= EVkBorderColor;
		using ESamplerMipmapMode	= EVkSamplerMipmapMode;
		using ESamplerAddressMode	= EVkSamplerAddressMode;
		using ECullMode             = EVkCullMode;
		using ESwizzle              = EVkSwizzle;
		using ECompareMode          = EVkCompareOp;
    };

    void RHI::
    Bootstrap()
    {
        VE_LOG_TRACE("Creating Loader...");
        Loader   = FLoader::Create();
        VE_LOG_TRACE("Creating Instance...");
        Instance = FInstance::Create(Loader);
        // Find Suitable GPU (Discreted? Surface Format?)

        // Device

        // Optional(Surface & Swapchain)

        //...
    }

    void RHI::
    Terminate()
    {
        VE_LOG_TRACE("Destroying Instance...");
        Instance.reset();
        VE_LOG_TRACE("Destroying Loader...");
        Loader.reset();
    }

} // namespace VE