module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.RHI.Vulkan:DescriptorSet;

import :Common;
import :Device;
import :DescriptorSetLayout;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class FVulkanDescriptorPool;

	class FVulkanDescriptorSet
	{
		friend class FVulkanDescriptorPool;
	public:
		enum class EStatus { Expired, Idle, Updating };

		Bool IsExpired() const { return Status == EStatus::Expired; }

		FVulkanDescriptorSet() = delete;
		FVulkanDescriptorSet(SharedPtr<FVulkanDescriptorPool> _Owner, SharedPtr<FVulkanDescriptorSetLayout> _Layout);
		~FVulkanDescriptorSet() = default; // Recollected via its owner automatically.
	private:
		EStatus			 Status { EStatus::Idle  };
		VkDescriptorSet  Handle { VK_NULL_HANDLE };
		SharedPtr<FVulkanDescriptorPool>	  Owner;
		SharedPtr<FVulkanDescriptorSetLayout> Setting;
	};

	FVulkanDescriptorSet::
	FVulkanDescriptorSet(SharedPtr<FVulkanDescriptorPool> _Owner, SharedPtr<FVulkanDescriptorSetLayout> _Layout)
		: Owner{ std::move(_Owner) },
		  Setting { std::move(_Layout) }
	{
		VE_ASSERT(Setting->GetHandle() != VK_NULL_HANDLE);
	}
	
} } // namespace VE::Runtime