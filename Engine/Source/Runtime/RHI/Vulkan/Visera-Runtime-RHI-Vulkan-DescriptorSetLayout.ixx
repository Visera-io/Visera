module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSetLayout;

import :Common;
import :Device;

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
			//SharedPtr<FVulkanSampler> Sampler;
			const VkSampler* Samplers = nullptr;
		};

		auto GetHandle() const -> const VkDescriptorSetLayout { return Handle; }

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
					   [](const auto& _BindingSlot)->VkDescriptorSetLayoutBinding
						{
							VE_ASSERT(_BindingSlot.BindPoint >= 0);
							return VkDescriptorSetLayoutBinding
							{
								.binding		 = UInt32(_BindingSlot.BindPoint),
								.descriptorType  = AutoCast(_BindingSlot.DescriptorType),
								.descriptorCount = _BindingSlot.DescriptorCount,
								.stageFlags		 = AutoCast(_BindingSlot.ShaderStages),
								.pImmutableSamplers = _BindingSlot.Samplers,
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