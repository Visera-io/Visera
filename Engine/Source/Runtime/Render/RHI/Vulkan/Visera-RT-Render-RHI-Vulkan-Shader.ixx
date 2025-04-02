module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:Shader;
import :Context;
import Visera.Core.Signal;

import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;

export namespace VE
{
		
	class FVulkanShader
	{
	public:
		auto GetEntryPoint()	const -> StringView				{ return "main"; }
		auto GetStage()			const -> EVulkanShaderStage		{ return Stage; }

		Bool IsExpired() const { return Handle == VK_NULL_HANDLE; }
		void Release() { VE_ASSERT(!IsExpired()); vkDestroyShaderModule(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks); Handle = VK_NULL_HANDLE; }

		auto GetHandle()			const { return Handle; }
		operator VkShaderModule()	const { return Handle; }

	public:
		FVulkanShader() noexcept = delete;
		FVulkanShader(EVulkanShaderStage _ShaderStage, const void* _SPIRVCode, UInt64 _CodeSize);
		~FVulkanShader() noexcept;
		
	private:
		VkShaderModule			Handle{ VK_NULL_HANDLE };
		//String				EntryPoint; (Always "main")
		EVulkanShaderStage		Stage;
	};

	FVulkanShader::
	FVulkanShader(EVulkanShaderStage _ShaderStage,
				  const void* _SPIRVCode,
				  UInt64 _CodeSize)
		:Stage{_ShaderStage}
	{
		VE_ASSERT(_SPIRVCode && _CodeSize);
		VkShaderModuleCreateInfo CreateInfo
		{
			.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize	= _CodeSize,
			.pCode		= reinterpret_cast<const uint32_t*>(_SPIRVCode)
		};
		if(vkCreateShaderModule(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ throw SRuntimeError("Failed to create Vulkan Shader Module!"); }
	}

	FVulkanShader::
	~FVulkanShader() noexcept
	{
		if (!IsExpired()) { Release(); }
	}

} // namespace VE