module;
#include <ViseraEngine>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Visera.Render.RHI.Vulkan;
import :Loader;

import Visera.Core.Log;
import Visera.Platform.Window;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class RHI;

	class VulkanContext
	{
		friend class RHI;
	public:
		/* << Vulkan Objects >>
		   1. Instance
		   2. Messenger
		*/
		struct {
			RawString					AppName		= VISERA_ENGINE_NAME;
			uint32_t					AppVersion	= VK_MAKE_VERSION(1, 0, 0);
			VkInstance					Handle		{ VK_NULL_HANDLE };
			
			UInt32						APIVersion  = VK_API_VERSION_1_3;
			Array<RawString>			Layers;
			Array<RawString>			Extensions;
			
			struct {
				VkDebugUtilsMessengerEXT			 Handle{ VK_NULL_HANDLE };
				PFN_vkDebugUtilsMessengerCallbackEXT Callback = DefaultMessengerCallback;
				operator VkDebugUtilsMessengerEXT() const { return Handle; }
			}Messenger;

			operator VkInstance() const { return Handle; }
		}Instance;

		struct
		{
			GLFWwindow*				Handle = nullptr;
			operator GLFWwindow* () const { return Handle; }
		}Window;

		struct {
			VkSurfaceKHR                Handle{ VK_NULL_HANDLE };
			Array<VkSurfaceFormatKHR>	Formats;
			Array<VkPresentModeKHR>		PresentModes;
			operator VkSurfaceKHR() const { return Handle; }
		}Surface{};

		struct {
			VkDevice			Handle;
			operator VkDevice() const { return Handle; }
		}Device;

		const VkAllocationCallbacks* AllocationCallbacks  { nullptr };

	private:
		UniquePtr<VulkanLoader> Volk;
		//UniquePtr<VulkanMemoryAllocator> VMA;

	public:
		VulkanContext()
		{
			Volk = CreateUniquePtr<VulkanLoader>();

			CreateInstance();
			CreateSurface();
			//CreateDevice();
		}

		~VulkanContext()
		{
			//DestroyDevice();
			DestroySurface();
			DestroyInstance();

			Volk.reset();
		}

		void CreateInstance()
		{
			// Layers
			Array<RawString> EnabledLayers
			{
				"VK_LAYER_KHRONOS_validation",
			};
			Instance.Layers.resize(EnabledLayers.size());
			for (Int32 i = 0; i < Instance.Layers.size(); ++i)
			{ Instance.Layers[i] = EnabledLayers[i]; }

			// Extensions
			Array<RawString> EnabledExtensions
			{
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			};

			// GLFW Necessary Extensions
			UInt32 GLFWExtensionCount = 0;
			const char** GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount); // Include WSI extensions

			UInt32 TotalExtensionCount = GLFWExtensionCount + EnabledExtensions.size();
			Instance.Extensions.resize(TotalExtensionCount);
			auto It = Instance.Extensions.begin();
			for (const auto& Extension : EnabledExtensions)
			{ *It = Extension; ++It; }
			for (UInt32 i = 0; i < GLFWExtensionCount; ++i)
			{ *It = *(GLFWExtensions + i); ++It; }

			UInt32 LayerPropertyCount = 0;
			VK_CHECK(vkEnumerateInstanceLayerProperties(&LayerPropertyCount, nullptr));

			Array<VkLayerProperties> LayerProperties(LayerPropertyCount);
			vkEnumerateInstanceLayerProperties(&LayerPropertyCount, LayerProperties.data());
			
			for (auto RequiredLayer : Instance.Layers)
			{
				bool Found = False;
				for (const auto& AvailableLayer : LayerProperties)
				{
					if (strcmp(RequiredLayer, AvailableLayer.layerName) == 0)
					{ Found = true; break; }
				}
				if (!Found) Log::Fatal("Failed to enable the Vulkan Validation Layer {}", RequiredLayer);
			}

			VkApplicationInfo AppInfo
			{
				.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pApplicationName	= Instance.AppName,
				.applicationVersion = Instance.AppVersion,
				.apiVersion			= Instance.APIVersion,
			};

			Address Next{ nullptr };
#ifndef NDEBUG
			VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity=  
					//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType    =      
					//VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback= Instance.Messenger.Callback,
				.pUserData      = nullptr
			};
			Next = Address(&MessengerCreateInfo);
#else
			
#endif
			VkInstanceCreateInfo InstanceCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext = Next,
				.pApplicationInfo		= &AppInfo,
				.enabledLayerCount		= UInt32(Instance.Layers.size()),
				.ppEnabledLayerNames	= Instance.Layers.data(),
				.enabledExtensionCount	= UInt32(Instance.Extensions.size()),
				.ppEnabledExtensionNames= Instance.Extensions.data(),
			};
			VK_CHECK(vkCreateInstance(&InstanceCreateInfo, AllocationCallbacks, &Instance.Handle));

			Volk->LoadInstance(Instance);
		}

		void DestroyInstance()
		{
#ifndef NDEBUG
			auto LoadedDestroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
			Assert(LoadedDestroyFunc && "Failed to load function: vkDestroyDebugUtilsMessengerEXT");
			LoadedDestroyFunc(Instance, Instance.Messenger, AllocationCallbacks);
#endif
			vkDestroyInstance(Instance, AllocationCallbacks);
			Instance.Handle = VK_NULL_HANDLE;
		}

		void CreateDevice()
		{
			Volk->LoadDevice(Device);
		}

		void DestroyDevice()
		{

		}

		void CreateSurface()
		{
			VK_CHECK(glfwCreateWindowSurface(
				Instance,
				Window::GetHandle(),
				AllocationCallbacks,
				&Surface.Handle));
		}

		void DestroySurface()
		{
			vkDestroySurfaceKHR(Instance, Surface, AllocationCallbacks);
			Surface.Handle = VK_NULL_HANDLE;
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL
			DefaultMessengerCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT MessageType,
				const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
				void* pUserData)
		{
			switch (MessageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				Log::Debug("[Vulkan]: {}",	CallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				Log::Warn("[Vulkan]: {}",	CallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				Log::Fatal("[Vulkan]: {}",	CallbackData->pMessage);
				break;
			default:
				Log::Error("[Vulkan]: Unknow Message Severity {}", ErrorCode(MessageSeverity));
			}
			return VK_FALSE; // Always return VK_FALSE
		}
	};

} // namespace VE