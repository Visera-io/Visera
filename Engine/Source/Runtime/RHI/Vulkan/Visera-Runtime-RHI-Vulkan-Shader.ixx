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
		auto GetEntryPoint()	const -> StringView				{ return EntryPoint; }
		auto GetStage()			const -> EVulkanShaderStage		{ return Stage; }

		auto GetHandle()			const { return Handle; }
		operator VkShaderModule()	const { return Handle; }

	public:
		FVulkanShader() noexcept = delete;
		FVulkanShader(EVulkanShaderStage _ShaderStage, StringView _EntryPoint, const void* _SPIRVCode, UInt64 _CodeSize);
		~FVulkanShader() noexcept;
		
	private:
		VkShaderModule			Handle{ VK_NULL_HANDLE };
		String					EntryPoint;
		EVulkanShaderStage		Stage;
	};

	FVulkanShader::
	FVulkanShader(EVulkanShaderStage _ShaderStage,
				  StringView _EntryPoint,
				  const void* _SPIRVCode,
				  UInt64 _CodeSize)
		:Stage{_ShaderStage},
		 EntryPoint{_EntryPoint}
	{
		VE_ASSERT(_SPIRVCode && _CodeSize);
		VkShaderModuleCreateInfo CreateInfo
		{
			.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize	= _CodeSize,
			.pCode		= reinterpret_cast<const uint32_t*>(_SPIRVCode)
		};
		if(VK_SUCCESS != vkCreateShaderModule(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle))
		{ throw SRuntimeError("Failed to create Vulkan Shader Module!"); }
	}

	FVulkanShader::
	~FVulkanShader() noexcept
	{
		vkDestroyShaderModule(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime