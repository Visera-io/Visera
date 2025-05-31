module;
#include <Visera.h>
#include "VISERA_MODULE_LOCAL.H"
export module Visera.Runtime.Render.RHI.Core:Instance;
#define VE_MODULE_NAME "RHI.Core:Instance"

import Visera.Core.Log;

import :Loader;
import :GPU;

export namespace VE
{
    class FInstance
    {
	public:
		[[nodiscard]] static inline auto
		Create(SharedPtr<FLoader>& _Loader) { return CreateSharedPtr<FInstance>(_Loader); }

	public:
		[[nodiscard]] Array<FGPU>
		EnumerateAvailableGPUs()	const;
		[[nodiscard]] UInt32
		GetVulkanAPIVersion()		const { return APIVersion; }
		[[nodiscard]] VkInstance
		GetHandle()	                const { return  Handle; }

	private:
		VkInstance				Handle		= VK_NULL_HANDLE;

		RawString				AppName		= VISERA_ENGINE_NAME;
		UInt32					AppVersion	= VK_MAKE_VERSION(1, 0, 0);
		UInt32					APIVersion;

		Array<RawString>		Layers
								{
#if (NDEBUG)

#else
                                "VK_LAYER_KHRONOS_validation",
#endif
								};

		Array<RawString>		Extensions
								{
								// Implicitly import some platform surface extensions via glFW.
#if defined(VE_ON_APPLE_SYSTEM)
								VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif

#if (NDEBUG)
								// Release Extensions
#else
								// Debug Extensions
								VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
								VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
								};
		VkFlags					Flags = 0
#if defined(VE_ON_APPLE_SYSTEM)
								VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
#endif
								;
			
		struct
		{
			VkDebugUtilsMessengerEXT			 Handle{ VK_NULL_HANDLE };
			PFN_vkDebugUtilsMessengerCallbackEXT Callback = DefaultMessengerCallback;
		} Messenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL
			DefaultMessengerCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT MessageType,
				const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
				void* pUserData)
		{
			if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				VE_LOG_TRACE("{}", CallbackData->pMessage);
			}
			else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				VE_LOG_DEBUG("{}", CallbackData->pMessage);
			}
			else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				VE_LOG_WARN("{}", CallbackData->pMessage);
			}
			else if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				VE_LOG_ERROR("{}", CallbackData->pMessage);
			}
			else
			{
				VE_LOG_FATAL("Unknown Message Severity {}", ErrorCode(MessageSeverity));
			}
			return VK_FALSE; // Always return VK_FALSE
		}
		
	public:
		FInstance() noexcept = delete;
		FInstance(SharedPtr<FLoader>& _Loader) noexcept;
		~FInstance() noexcept;
	};

	FInstance::
	FInstance(SharedPtr<FLoader>& _Loader) noexcept
	{
		// GLFW Necessary Extensions
		UInt32 GLFWExtensionCount = 0;
		const char** GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount); // Include WSI extensions
		for (UInt32 i = 0; i < GLFWExtensionCount; ++i) { Extensions.emplace_back(*(GLFWExtensions + i)); }

		UInt32 LayerPropertyCount = 0;
		if(vkEnumerateInstanceLayerProperties(&LayerPropertyCount, nullptr) != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to enumerate instance layer properties!"); }

		Array<VkLayerProperties> LayerProperties(LayerPropertyCount);
		vkEnumerateInstanceLayerProperties(&LayerPropertyCount, LayerProperties.data());

		for (auto RequiredLayer : Layers)
		{
			bool Found = False;
			for (const auto& AvailableLayer : LayerProperties)
			{
				if (strcmp(RequiredLayer, AvailableLayer.layerName) == 0)
				{ Found = true; break; }
			}
			if (!Found)
			{ VE_LOG_FATAL("Failed to enable the Vulkan Validation Layer {}", RequiredLayer); }
		}

		const VkApplicationInfo AppInfo
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName	= AppName,
			.applicationVersion = AppVersion,
			.pEngineName		= VISERA_ENGINE_NAME,
			.engineVersion		= VK_MAKE_VERSION(1,0,0),
			.apiVersion			= _Loader->GetAPIVersion(),
		};

		Address Next{ nullptr };
	#ifndef NDEBUG
		VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity =
				//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType     =
				//VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = Messenger.Callback,
			.pUserData       = nullptr
		};
		Next = Address(&MessengerCreateInfo);
	#else
			
	#endif

		VkInstanceCreateInfo InstanceCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = Next,
			.flags = Flags,
			.pApplicationInfo		= &AppInfo,
			.enabledLayerCount		= UInt32(Layers.size()),
			.ppEnabledLayerNames	= Layers.data(),
			.enabledExtensionCount	= UInt32(Extensions.size()),
			.ppEnabledExtensionNames= Extensions.data(),
		};
		auto Result = vkCreateInstance(&InstanceCreateInfo,
										AllocationCallbacks,
										&Handle);
		if(Result != VK_SUCCESS)
		{ VE_LOG_FATAL("Failed to create Vulkan Instance ({})!", Int32(Result)); }

		_Loader->LoadInstance(Handle);
	}

	FInstance::
	~FInstance() noexcept
	{
	#ifndef NDEBUG
		auto DestroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Handle, "vkDestroyDebugUtilsMessengerEXT");
		VE_ASSERT(DestroyFunc && "Failed to load function: vkDestroyDebugUtilsMessengerEXT");
		DestroyFunc(Handle, Messenger.Handle, AllocationCallbacks);
	#endif
		vkDestroyInstance(Handle, AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

} // namespace VE