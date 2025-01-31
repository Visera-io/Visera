module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSetLayout;

import :Enums;
import :Device;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class FVulkanDescriptorSetLayout
	{
	public:
		struct FBinding
		{
			EDescriptorType DescriptorType;
			UInt32			DescriptorCount;
			EShaderStage    ShaderStages;
			//SharedPtr<FVulkanSampler> Sampler;
			const VkSampler* Samplers = nullptr;
		};

		auto GetHandle() -> VkDescriptorSetLayout { return Handle; }

		FVulkanDescriptorSetLayout() = delete;
		FVulkanDescriptorSetLayout(Array<FBinding> _Bindings);
		~FVulkanDescriptorSetLayout();
	private:
		VkDescriptorSetLayout  Handle{ VK_NULL_HANDLE };
		Array<FBinding>	       BindingSlots;
	};

	FVulkanDescriptorSetLayout::
	FVulkanDescriptorSetLayout(Array<FBinding> _Bindings)
		: BindingSlots { std::move(_Bindings) }
	{
		VE_ASSERT(BindingSlots.size() > 0);

		Array<VkDescriptorSetLayoutBinding> Bindings(BindingSlots.size());
		for (UInt32 Idx = 0; Idx < Bindings.size(); ++Idx)
		{
			Bindings[Idx] = VkDescriptorSetLayoutBinding
			{
				.binding = Idx,
				.descriptorType  = AutoCast(BindingSlots[Idx].DescriptorType),
				.descriptorCount = BindingSlots[Idx].DescriptorCount,
				.stageFlags		 = AutoCast(BindingSlots[Idx].ShaderStages),
				.pImmutableSamplers = BindingSlots[Idx].Samplers,
			};
		}

		VkDescriptorSetLayoutCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = UInt32(Bindings.size()),
			.pBindings    = Bindings.data()
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
	
} } // namespace VE::Runtime