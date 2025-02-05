module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:Shader;

import Visera.Core.Signal;

import :Common;
import :Device;

export namespace VE { namespace Runtime
{

	class FVulkanShader
	{
		friend class FVulkan;
	public:
		auto GetEntryPoint()	const -> RawString				{ VE_WIP; return "main"; }
		auto GetSize()			const -> VkDeviceSize			{ return Data.size(); }
		auto GetData()			const -> const Array<Byte>&		{ return Data; }
		auto GetStage()			const -> VkShaderStageFlagBits	{ return Stage; }

		auto GetHandle()			const { return Handle; }
		operator VkShaderModule()	const { return Handle; }

	public:
		FVulkanShader() noexcept = default;
		FVulkanShader(EShaderStage ShaderType, const Array<Byte>& ShadingCode);
		~FVulkanShader() noexcept;
		
	private:
		VkShaderModule			Handle{ VK_NULL_HANDLE };
		VkShaderStageFlagBits	Stage;
		Array<Byte>				Data;
	};

	FVulkanShader::
	FVulkanShader(EShaderStage ShaderType, const Array<Byte>& ShadingCode)
		:Stage{VkShaderStageFlagBits(ShaderType)}, Data{ShadingCode}
	{
		VE_ASSERT(!Data.empty());
		VkShaderModuleCreateInfo CreateInfo
		{
			.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize	= Data.size(),
			.pCode		= reinterpret_cast<const uint32_t*>(Data.data())
		};
		if(VK_SUCCESS != vkCreateShaderModule(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan FShader Module!"); }
	}

	FVulkanShader::
	~FVulkanShader() noexcept
	{
		vkDestroyShaderModule(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime