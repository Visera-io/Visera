module;
#include "../VulkanPC.h"
export module Visera.Runtime.Render.RHI.Vulkan:Shader;

import :Enums;
import :Device;

VISERA_PUBLIC_MODULE

class VulkanShader
{
	friend class Vulkan;
public:
	auto GetName()	const -> RawString				{ return "main"; }
	auto GetSize()  const -> VkDeviceSize			{ return Data.size(); }
	auto GetData()  const -> const Array<Byte>&		{ return Data; }
	auto GetStage() const -> VkShaderStageFlagBits	{ return Stage; }

	auto GetHandle()			const { return Handle; }
	operator VkShaderModule()	const { return Handle; }

public:
	VulkanShader() noexcept = delete;
	VulkanShader(EShaderStage ShaderType, const Array<Byte>& ShadingCode);
	~VulkanShader() noexcept;
		
private:
	VkShaderModule			Handle{ VK_NULL_HANDLE };
	VkShaderStageFlagBits	Stage;
	Array<Byte>				Data;
};

VulkanShader::
VulkanShader(EShaderStage ShaderType, const Array<Byte>& ShadingCode)
	:Stage{VkShaderStageFlagBits(ShaderType)}, Data{ShadingCode}
{
	VE_ASSERT(!Data.empty());
	VkShaderModuleCreateInfo CreateInfo
	{
		.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize	= Data.size(),
		.pCode		= reinterpret_cast<const uint32_t*>(Data.data())
	};
	VK_CHECK(vkCreateShaderModule(GVulkan->Device->GetHandle(), &CreateInfo, GVulkan->AllocationCallbacks, &Handle));
}

VulkanShader::
~VulkanShader() noexcept
{
	vkDestroyShaderModule(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

VISERA_MODULE_END