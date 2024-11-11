module;
#include <Visera>

#include <volk.h>

export module Visera.Engine.Runtime.Render.RHI.Vulkan:PipelineCache;

import :Context;
import :Allocator;
import :Device;

import Visera.Engine.Core.IO;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanPipelineCache
	{
		friend class Vulkan;
	public:
		Bool IsExpired() const { return bExpired; }
		auto GetHandle() const -> VkPipelineCache { return Handle; }
		operator VkPipelineCache() const { return Handle; }

	private:
		VkPipelineCache Handle{ VK_NULL_HANDLE };
		RawString		Path;
		Bool			bExpired = False;
		
	private:
		void Create();
		void Destroy();
		VulkanPipelineCache() noexcept = delete;
		VulkanPipelineCache(RawString CachePath) noexcept : Path{ CachePath } {};
		~VulkanPipelineCache() noexcept = default;
	};

	void VulkanPipelineCache::
	Create()
	{
		auto CacheFile = IO::CreateBinaryFile(Path);

	}

	void VulkanPipelineCache::
	Destroy()
	{

	}

} } // namespace VE::Runtime