module;
#include <Visera.h>
#define  VOLK_IMPLEMENTATION
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Core:Loader;
#define VE_MODULE_NAME "RHI.Core:Loader"

import Visera.Core.Log;

//#if defined(VK_VERSION_1_4)
//#define API_VERSION VK_API_VERSION_1_4
#if defined(VK_VERSION_1_3)
#define API_VERSION VK_API_VERSION_1_3
#elif defined(VK_VERSION_1_2)
#define API_VERSION VK_API_VERSION_1_2
#elif defined(VK_VERSION_1_1)
#define API_VERSION VK_API_VERSION_1_1
#else
#define API_VERSION VK_API_VERSION_1_0
#endif

export namespace VE
{
	
    class FLoader
	{
	public:
		[[nodiscard]] static inline auto
		Create() { return CreateSharedPtr<FLoader>(); }

		auto inline
		GetAPIVersion() const { return API_VERSION; }
		void inline
		LoadInstance(VkInstance Instance) const;
		void inline
		LoadDevice(VkDevice Device) const;

	public:
		FLoader()  noexcept;
		~FLoader() noexcept;
	};

	FLoader::
	FLoader() noexcept
	{
		if (volkInitialize() != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to initialize Volk!"); }
	}

	FLoader::
	~FLoader() noexcept
	{
		volkFinalize();
	}
		
	void FLoader::
	LoadInstance(VkInstance Instance) const
	{
		VE_ASSERT(Instance != VK_NULL_HANDLE);
		volkLoadInstance(Instance);
	}

	void FLoader::
	LoadDevice(VkDevice Device) const
	{
		VE_ASSERT(Device != VK_NULL_HANDLE);
		volkLoadDevice(Device);
	}

} // namespace VE