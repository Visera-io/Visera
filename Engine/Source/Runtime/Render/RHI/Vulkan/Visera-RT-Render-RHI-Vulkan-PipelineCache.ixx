module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:PipelineCache;
#define VE_MODULE_NAME "Vulkan:PipelineCache"
import :Context;
import :Device;
import :GPU;

import Visera.Core.OS.FileSystem;
import Visera.Core.Log;

export namespace VE
{

	class FVulkanPipelineCache
	{
		friend class FVulkan;
	public:
		Bool IsExpired() const { return bExpired; }
		auto GetHandle() const -> const VkPipelineCache { return Handle; }

	private:
		VkPipelineCache Handle{ VK_NULL_HANDLE };
		FPath			Path;
		Bool			bExpired = False;
		
	private:
		void Create();
		void Destroy();
		FVulkanPipelineCache() noexcept = delete;
		FVulkanPipelineCache(FPath _CachePath) noexcept : Path{ std::move(_CachePath) } {};
		~FVulkanPipelineCache() noexcept = default;
	};

	void FVulkanPipelineCache::
	Create()
	{
		FileSystem::CreateFileIfNotExists(Path);
		auto CacheFile = FileSystem::CreateBinaryFile(Path);
		CacheFile->Load();

		auto* CacheHeader = (VkPipelineCacheHeaderVersionOne*)(CacheFile->GetData().data());
		const auto& GPUProperties = GVulkan->GPU->GetProperties();

		bExpired = (CacheFile->IsEmpty() ||
					CacheHeader->deviceID != GPUProperties.deviceID ||
					CacheHeader->vendorID != GPUProperties.vendorID ||
					memcmp(CacheHeader->pipelineCacheUUID, GPUProperties.pipelineCacheUUID, VK_UUID_SIZE));
		
		if (IsExpired()) { CacheFile->Clear(); }
		VkPipelineCacheCreateInfo CreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			.initialDataSize	= CacheFile->GetSize(),
			.pInitialData		= CacheFile->GetData().data(),
		};
		if(vkCreatePipelineCache(
			GVulkan->Device->GetHandle(),
			&CreateInfo,
			GVulkan->AllocationCallbacks,
			&Handle) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to create Vulkan Pipeline Cache!"); }
	}

	void FVulkanPipelineCache::
	Destroy()
	{
		if (IsExpired())
		{
			auto CacheFile = FileSystem::CreateBinaryFile(Path);
			std::size_t CacheSize = 0;

			vkGetPipelineCacheData(GVulkan->Device->GetHandle(), Handle, &CacheSize, nullptr);
			CacheFile->Resize(CacheSize);
			vkGetPipelineCacheData(GVulkan->Device->GetHandle(), Handle, &CacheSize, CacheFile->Access());
			CacheFile->Save();
		}
		vkDestroyPipelineCache(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE