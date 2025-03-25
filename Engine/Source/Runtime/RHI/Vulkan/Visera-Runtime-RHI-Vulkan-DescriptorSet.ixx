module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSet;
import :Context;
import Visera.Runtime.RHI.Vulkan.Common;
import :Allocator;
import :Sampler;
import :Device;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE
{
	class FVulkanDescriptorPool;

	class FVulkanDescriptorSet
	{
		friend class FVulkanDescriptorPool;
	public:
		enum class EStatus { Expired, Idle, Updating };

		void WriteImage(UInt32 _BindPoint, SharedPtr<const FVulkanImageView> _ImageView, SharedPtr<const FVulkanSampler> _Sampler = nullptr);

		Bool IsExpired() const { return Status == EStatus::Expired; }

		auto GetHandle() const -> const VkDescriptorSet { return Handle; }

		FVulkanDescriptorSet() = delete;
		FVulkanDescriptorSet(SharedPtr<FVulkanDescriptorPool> _Owner, SharedPtr<const FVulkanDescriptorSetLayout> _Layout);
		~FVulkanDescriptorSet() = default; // Recollected via its owner automatically.
	private:
		EStatus			 Status { EStatus::Idle  };
		VkDescriptorSet  Handle { VK_NULL_HANDLE };
		SharedPtr<FVulkanDescriptorPool>			Owner;
		SharedPtr<const FVulkanDescriptorSetLayout> Layout;
	};

	FVulkanDescriptorSet::
	FVulkanDescriptorSet(SharedPtr<FVulkanDescriptorPool> _Owner, SharedPtr<const FVulkanDescriptorSetLayout> _Layout)
		: Owner{ std::move(_Owner) },
		  Layout { std::move(_Layout) }
	{
		VE_ASSERT(Layout->GetHandle() != VK_NULL_HANDLE);
	}

	void FVulkanDescriptorSet::
	WriteImage(UInt32 _BindPoint, SharedPtr<const FVulkanImageView> _ImageView, SharedPtr<const FVulkanSampler> _Sampler/* = nullptr */)
	{
		if (_ImageView->IsExpired())
		{ throw SRuntimeError("Failed to write an expired VulkanImage to the descriptor set!"); }

		VkDescriptorImageInfo ImageInfo
		{
			.sampler		= _Sampler? _Sampler->GetHandle() : VK_NULL_HANDLE,
			.imageView		= _ImageView->GetHandle(),
			.imageLayout	= AutoCast(_ImageView->GetLayoutView()),
		};

		auto& TargetBinding = Layout->GetBinding(_BindPoint);
		VkWriteDescriptorSet WriteInfo
		{
			.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			= Handle,
			.dstBinding		= _BindPoint,
			.dstArrayElement= 0,
			.descriptorCount= 1,
			.descriptorType	= AutoCast(TargetBinding.DescriptorType),
			.pImageInfo		= &ImageInfo,
			.pBufferInfo	= nullptr,
			.pTexelBufferView = nullptr,
		};

		vkUpdateDescriptorSets(GVulkan->Device->GetHandle(), 1, &WriteInfo, 0, nullptr);
	}
	
} // namespace VE