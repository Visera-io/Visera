module;
#include <ViseraEngine>

#include <volk.h>

export module Visera.Runtime.Render.RHI.Vulkan:CommandPool;

import Visera.Core.Log;

export namespace VE { namespace RHI
{
	class VulkanContext;

	class VulkanCommandPool
	{
		friend class VulkanContext;
	public:
		class CommandBuffer
		{
		public:

		private:

		};

	private:


	public:
		VulkanCommandPool() noexcept = default;
		~VulkanCommandPool() noexcept = default;
	};

} } // namespace VE::RHI