module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:DescriptorSetLayout;
#define VE_MODULE_NAME "Vulkan:DescriptorSetLayout"
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
import :Device;
import :Sampler;

import Visera.Core.Log;

export namespace VE
{
	class FVulkanDescriptorPool;

	class FVulkanDescriptorSetLayout : public std::enable_shared_from_this<FVulkanDescriptorSetLayout>
	{//[TODO]: DesciptorSetLayout Hashing
		friend class FVulkanDescriptorPool;
	public:
		static inline auto
		Create()  { return CreateSharedPtr<FVulkanDescriptorSetLayout>(); }
		inline FVulkanDescriptorSetLayout*
		AddBinding(UInt8 _BindPoint, EVulkanDescriptorType _Type, UInt16 _Count, EVulkanShaderStage _ShaderStages); //[TODO]: ImmutableSamplers
		inline auto
		Build() -> SharedPtr<FVulkanDescriptorSetLayout>;
		inline void
		Destroy();

		Bool inline
		IsBuilt() const { return Handle != VK_NULL_HANDLE; }
		Bool inline
		HasBinding(UInt8 _BindPoint) const { return _BindPoint < GetBindingCount(); }

		auto GetHandle() const -> const VkDescriptorSetLayout { return Handle; }
		auto GetBindingCount() const -> UInt32 { return Bindings.size(); }
		auto GetBinding(UInt8 _BindPoint) const -> const VkDescriptorSetLayoutBinding& { VE_ASSERT(HasBinding(_BindPoint)); return Bindings[_BindPoint]; }

		FVulkanDescriptorSetLayout() = default;
		~FVulkanDescriptorSetLayout() { Destroy(); };
	private:
		VkDescriptorSetLayout  Handle{ VK_NULL_HANDLE };
		Array<VkDescriptorSetLayoutBinding> Bindings;
	};

	FVulkanDescriptorSetLayout* FVulkanDescriptorSetLayout::
	AddBinding(UInt8 _BindPoint, EVulkanDescriptorType _Type, UInt16 _Count, EVulkanShaderStage _ShaderStages)
	{
		Bindings.emplace_back(VkDescriptorSetLayoutBinding
			{
				.binding			= _BindPoint,
				.descriptorType		= AutoCast(_Type),
				.descriptorCount	= _Count,
				.stageFlags			= AutoCast(_ShaderStages),
				.pImmutableSamplers = nullptr,
			});

		return this;
	}

	SharedPtr<FVulkanDescriptorSetLayout> FVulkanDescriptorSetLayout::
	Build()
	{
		VE_ASSERT(!IsBuilt());

		VkDescriptorSetLayoutCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = UInt32(Bindings.size()),
			.pBindings    = Bindings.empty()? nullptr : Bindings.data()
		};

		if (vkCreateDescriptorSetLayout(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to create the FVulkanDescriptorSetLayout!"); }

		VE_LOG_DEBUG("Built a new descriptor set layout (handle:{}, bindings:{})", (Address)(Handle), GetBindingCount());
		return shared_from_this();
	}

	void FVulkanDescriptorSetLayout::
	Destroy()
	{
		if (IsBuilt())
		{
			vkDestroyDescriptorSetLayout(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
			Handle = VK_NULL_HANDLE;
		}
	}
	
} // namespace VE