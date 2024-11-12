module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Common;

export namespace VE { namespace Runtime
{
    #define ENUM_BIT(Flag, Bit) static constexpr VkFlags Flag = Bit;

    struct VulkanMemoryPermission
    {
        ENUM_BIT(None, VK_ACCESS_NONE)
        struct Read
        {
            ENUM_BIT(IndirectCommand,   VK_ACCESS_INDIRECT_COMMAND_READ_BIT)
            ENUM_BIT(Index,             VK_ACCESS_INDEX_READ_BIT)
            ENUM_BIT(VertexAttribute,   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
            ENUM_BIT(Uniform,           VK_ACCESS_UNIFORM_READ_BIT)
            ENUM_BIT(InputAttachment,   VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)
            ENUM_BIT(Shader,            VK_ACCESS_SHADER_READ_BIT)
            ENUM_BIT(ColorAttachment,   VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
            ENUM_BIT(DepthStencilAttachment, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT)
            ENUM_BIT(Transfer,          VK_ACCESS_TRANSFER_READ_BIT)
            ENUM_BIT(Host,              VK_ACCESS_HOST_READ_BIT)
            ENUM_BIT(Memory,            VK_ACCESS_MEMORY_READ_BIT)
        };
        struct Write
        {
            ENUM_BIT(Shader,            VK_ACCESS_SHADER_WRITE_BIT)
            ENUM_BIT(ColorAttachment,   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
            ENUM_BIT(DepthStencilAttachment, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
            ENUM_BIT(Transfer,          VK_ACCESS_TRANSFER_WRITE_BIT)
            ENUM_BIT(Host,              VK_ACCESS_HOST_WRITE_BIT)
            ENUM_BIT(Memory,            VK_ACCESS_MEMORY_WRITE_BIT)
        };
    };
} } // namespace VE::Runtime
