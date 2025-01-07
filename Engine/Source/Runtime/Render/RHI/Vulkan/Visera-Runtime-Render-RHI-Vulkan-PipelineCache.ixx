module;
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Vulkan:PipelineCache;

import :Device;
import :GPU;

import Visera.Core.IO;

VISERA_PUBLIC_MODULE

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
	IO::File::CreateIfNotExists(Path);
	auto CacheFile = IO::CreateBinaryFile(Path);
	CacheFile->Load();

	auto* CacheHeader = (VkPipelineCacheHeaderVersionOne*)(CacheFile->GetData().data());
	const auto& GPUProperties = GVulkan->GPU->GetProperties();

	bExpired = (CacheFile->IsEmpty() ||
				CacheHeader->deviceID != GPUProperties.deviceID ||
				CacheHeader->vendorID != GPUProperties.vendorID ||
				memcmp(CacheHeader->pipelineCacheUUID, GPUProperties.pipelineCacheUUID, VK_UUID_SIZE));
		
	if (IsExpired()) CacheFile->ClearData();
	VkPipelineCacheCreateInfo CreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.initialDataSize	= CacheFile->GetSize(),
		.pInitialData		= CacheFile->GetData().data(),
	};
	VK_CHECK(vkCreatePipelineCache(GVulkan->Device->GetHandle(), &CreateInfo, GVulkan->AllocationCallbacks, &Handle));
}

void VulkanPipelineCache::
Destroy()
{
	if (IsExpired())
	{
		auto CacheFile = IO::CreateBinaryFile(Path);
		UInt64 CacheSize = 0;
		vkGetPipelineCacheData(GVulkan->Device->GetHandle(), Handle, &CacheSize, nullptr);
		Array<Byte> CacheDate(CacheSize);
		vkGetPipelineCacheData(GVulkan->Device->GetHandle(), Handle, &CacheSize, CacheDate.data());
		CacheFile->WriteAll(std::move(CacheDate));
		CacheFile->Save();
	}
	vkDestroyPipelineCache(GVulkan->Device->GetHandle(), Handle, GVulkan->AllocationCallbacks);
	Handle = VK_NULL_HANDLE;
}

VISERA_MODULE_END