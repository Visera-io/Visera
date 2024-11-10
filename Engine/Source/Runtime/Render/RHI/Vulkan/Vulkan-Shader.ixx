module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:Shader;

import :Context;
import :Allocator;
import :Device;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanShader
	{
		friend class Vulkan;
	public:
		enum Stage
		{
			Vertex		= VK_SHADER_STAGE_VERTEX_BIT,
			Fragment	= VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		auto GetSize()  const -> VkDeviceSize			{ return Data.size(); }
		auto GetData()  const -> const Array<Byte>&		{ return Data; }
		auto GetStage() const -> VkShaderStageFlagBits	{ return Type; }
		auto GetType()	const -> Stage					{ return Stage(Type); }
				
		auto GetHandle()			const { return Handle; }
		operator VkShaderModule()	const { return Handle; }

	public:
		VulkanShader() noexcept = delete;
		VulkanShader(Stage ShaderType, const Array<Byte>& ShadingCode);
		~VulkanShader() noexcept;
		
	private:
		VkShaderModule			Handle{ VK_NULL_HANDLE };
		VkShaderStageFlagBits	Type;
		Array<Byte>				Data;
	};

	VulkanShader::
	VulkanShader(VulkanShader::Stage ShaderType, const Array<Byte>& ShadingCode)
		:Type{VkShaderStageFlagBits(ShaderType)}, Data{ShadingCode}
	{
		VkShaderModuleCreateInfo CreateInfo
		{
			.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize	= Data.size(),
			.pCode		= reinterpret_cast<const uint32_t*>(Data.data())
		};
		VK_CHECK(vkCreateShaderModule(GVulkan->Device->GetHandle(), &CreateInfo, VulkanAllocator::AllocationCallbacks, &Handle));
	}

	VulkanShader::
	~VulkanShader() noexcept
	{
		vkDestroyShaderModule(GVulkan->Device->GetHandle(), Handle, VulkanAllocator::AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} } // namespace VE::Runtime