module;
#include <ViseraEngine>

#include <vulkan/vulkan.h>

export module Visera.Render.RHI.Vulkan;

export namespace VE
{

	class Vulkan
	{
		friend class RHI;
	public:
		VkInstance	Instance	= VK_NULL_HANDLE;
		VkDevice	Device		= VK_NULL_HANDLE;

	private:
		Vulkan() noexcept = default;
		void Bootstrap()
		{

		}

		void Terminate()
		{

		}
	};

} // namespace VE