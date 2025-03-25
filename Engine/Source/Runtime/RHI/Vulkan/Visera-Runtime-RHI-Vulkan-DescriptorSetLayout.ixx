module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSetLayout;
import :Context;
import Visera.Runtime.RHI.Vulkan.Common;
import :Device;
import :Sampler;

import Visera.Core.Signal;

export namespace VE
{
	class FVulkanDescriptorPool;

	class FVulkanDescriptorSetLayout //[TODO]: DesciptorSetLayout Hashing
	{
		friend class FVulkanDescriptorPool;
	public:
		struct FBinding
		{
			Int8				  BindPoint{-1};
			EVulkanDescriptorType DescriptorType;
			UInt16				  DescriptorCount;
			EVulkanShaderStage    ShaderStages;
			Array<VkSampler>      ImmutableSamplers;
		};

		auto GetHandle() const -> const VkDescriptorSetLayout { return Handle; }
		auto GetBindingCount() const -> size_t { return BindingSlots.size(); }
		auto GetBinding(UInt32 _Position) const -> const FBinding& { VE_ASSERT(_Position < GetBindingCount()); return BindingSlots[_Position]; }

		FVulkanDescriptorSetLayout() = delete;
		FVulkanDescriptorSetLayout(const Array<FBinding>& _Bindings);
		~FVulkanDescriptorSetLayout();
	private:
		VkDescriptorSetLayout  Handle{ VK_NULL_HANDLE };
		Array<FBinding>	       BindingSlots;
	};

	FVulkanDescriptorSetLayout::
	FVulkanDescriptorSetLayout(const Array<FBinding>& _Bindings)
		: BindingSlots { _Bindings }
	{
		Array<VkDescriptorSetLayoutBinding> Bindings(BindingSlots.size());
		std::transform(BindingSlots.begin(), BindingSlots.end(),
					   Bindings.begin(),
					   [](const FBinding& _BindingSlot)->VkDescriptorSetLayoutBinding
						{
							VE_ASSERT(_BindingSlot.BindPoint >= 0);
			
							return VkDescriptorSetLayoutBinding
							{
								.binding		 = UInt32(_BindingSlot.BindPoint),
								.descriptorType  = AutoCast(_BindingSlot.DescriptorType),
								.descriptorCount = _BindingSlot.DescriptorCount,
								.stageFlags		 = AutoCast(_BindingSlot.ShaderStages),
								.pImmutableSamplers = _BindingSlot.ImmutableSamplers.empty()? nullptr : _BindingSlot.ImmutableSamplers.data(),
							};
						});

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
		{ throw SRuntimeError("Failed to create the FVulkanDescriptorSetLayout!"); }
	}

	FVulkanDescriptorSetLayout::
	~FVulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}
	
} // namespace VE