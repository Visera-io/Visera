module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:DescriptorSet;
import :Context;
import Visera.Runtime.Render.RHI.Vulkan.Common;
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

		void WriteImage(UInt32 _BindPoint, SharedPtr<const FVulkanImageView> _ImageView, SharedPtr<const FVulkanSampler> _Sampler);
		void WriteBuffer(UInt32 _BindPoint, SharedPtr<const FVulkanBuffer> _Buffer, UInt64 _Offset, UInt64 _Size);

		Bool IsExpired() const { return Status == EStatus::Expired; }

		auto GetHandle() const -> const VkDescriptorSet { return Handle; }
		auto GetOwner()  const -> SharedPtr<const FVulkanDescriptorPool> { return Owner; }

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
	WriteBuffer(UInt32 _BindPoint, SharedPtr<const FVulkanBuffer> _Buffer, UInt64 _Offset, UInt64 _Size)
	{
		if (!Layout->HasBinding(_BindPoint))
		{ throw SRuntimeError("Failed to write the buffer! -- Invaild BindPoint!"); }

		auto& TargetBinding = Layout->GetBinding(_BindPoint);

		VkDescriptorBufferInfo BufferInfo
		{
			.buffer = _Buffer->GetHandle(),
			.offset = _Offset,
			.range  = _Size,
		};
		
		VkWriteDescriptorSet WriteInfo
		{
			.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			= Handle,
			.dstBinding		= _BindPoint,
			.dstArrayElement= 0,
			.descriptorCount= 1,
			.descriptorType	= TargetBinding.descriptorType,
			.pImageInfo		= nullptr,
			.pBufferInfo	= &BufferInfo,
			.pTexelBufferView = nullptr,
		};

		vkUpdateDescriptorSets(GVulkan->Device->GetHandle(), 1, &WriteInfo, 0, nullptr);
	}

	void FVulkanDescriptorSet::
	WriteImage(UInt32 _BindPoint, SharedPtr<const FVulkanImageView> _ImageView, SharedPtr<const FVulkanSampler> _Sampler)
	{
		if (!Layout->HasBinding(_BindPoint))
		{ throw SRuntimeError("Failed to write the image! -- Invaild BindPoint!"); }

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
			.descriptorType	= TargetBinding.descriptorType,
			.pImageInfo		= &ImageInfo,
			.pBufferInfo	= nullptr,
			.pTexelBufferView = nullptr,
		};

		vkUpdateDescriptorSets(GVulkan->Device->GetHandle(), 1, &WriteInfo, 0, nullptr);
	}
	
} // namespace VE