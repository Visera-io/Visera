module;
#include <Visera>

#define VK_NO_PROTOTYPES
#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


export module Visera.Engine.Runtime.Render.RHI.Vulkan:Instance;

import Visera.Engine.Core.Log;
import :Context;
import :Loader;
import :GPU;

export namespace VE { namespace Runtime
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class Vulkan;

	class VulkanInstance
	{
		friend class Vulkan;
	public:
		auto EnumerateAvailableGPUs() const -> Array<VulkanGPU>;
		auto GetVulkanAPIVersion() const -> UInt32 { return AppVersion; }

		auto GetHandle() const		-> VkInstance { return  Handle; }
		operator VkInstance() const { return Handle; }

	private:
		RawString				AppName		= VISERA_ENGINE_NAME;
		UInt32					AppVersion	= VK_MAKE_VERSION(1, 0, 0);
		VkInstance				Handle		{ VK_NULL_HANDLE };
			
		UInt32					APIVersion  = VK_API_VERSION_1_3;
		Array<RawString>		Layers;
		Array<RawString>		Extensions;
			
		struct {
			VkDebugUtilsMessengerEXT			 Handle{ VK_NULL_HANDLE };
			PFN_vkDebugUtilsMessengerCallbackEXT Callback = DefaultMessengerCallback;
			operator VkDebugUtilsMessengerEXT() const { return Handle; }
		}Messenger;

		auto Create()	-> VkInstance;
		void Destroy();

	public:
		VulkanInstance() noexcept = default;
		~VulkanInstance() noexcept = default;

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
				Log::Fatal(std::format("[Vulkan]: {}",	CallbackData->pMessage));
				break;
			default:
				Log::Error("[Vulkan]: Unknow Message Severity {}", ErrorCode(MessageSeverity));
			}
			return VK_FALSE; // Always return VK_FALSE
		}
	};

	VkInstance VulkanInstance::
	Create()
	{
		Assert(VK_API_VERSION_1_3 != 0);
		// Layers
		Array<RawString> EnabledLayers
		{
			"VK_LAYER_KHRONOS_validation",
		};
		Layers.resize(EnabledLayers.size());
		for (Int32 i = 0; i < Layers.size(); ++i)
		{ Layers[i] = EnabledLayers[i]; }

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

		for (const auto& Extension : EnabledExtensions) { Extensions.emplace_back(Extension); }
		for (UInt32 i = 0; i < GLFWExtensionCount; ++i) { Extensions.emplace_back(*(GLFWExtensions + i)); }

		UInt32 LayerPropertyCount = 0;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&LayerPropertyCount, nullptr));

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
			if (!Found) Log::Fatal(std::format("Failed to enable the Vulkan Validation Layer {}", RequiredLayer));
		}

		const VkApplicationInfo AppInfo
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName	= AppName,
			.applicationVersion = AppVersion,
			.apiVersion			= APIVersion,
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
			.pfnUserCallback= Messenger.Callback,
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
			.enabledLayerCount		= UInt32(Layers.size()),
			.ppEnabledLayerNames	= Layers.data(),
			.enabledExtensionCount	= UInt32(Extensions.size()),
			.ppEnabledExtensionNames= Extensions.data(),
		};
		VK_CHECK(vkCreateInstance(&InstanceCreateInfo, GVulkan->AllocationCallbacks, &Handle));

		return Handle;
	}

	void VulkanInstance::
	Destroy()
	{
#ifndef NDEBUG
		auto LoadedDestroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Handle, "vkDestroyDebugUtilsMessengerEXT");
		Assert(LoadedDestroyFunc && "Failed to load function: vkDestroyDebugUtilsMessengerEXT");
		LoadedDestroyFunc(Handle, Messenger, GVulkan->AllocationCallbacks);
#endif
		vkDestroyInstance(Handle, GVulkan->AllocationCallbacks);
		Handle = VK_NULL_HANDLE;
	}

	Array<VulkanGPU> VulkanInstance::
	EnumerateAvailableGPUs() const
	{
		/*Find Proper Physical Device(Host)*/
		UInt32 PhysicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(Handle, &PhysicalDeviceCount, nullptr);
		if (!PhysicalDeviceCount) Log::Fatal("Failed to enumerate GPUs with Vulkan support!");

		Array<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
		vkEnumeratePhysicalDevices(Handle, &PhysicalDeviceCount, PhysicalDevices.data());

		Array<VulkanGPU> GPUs(PhysicalDevices.size());
		std::transform(PhysicalDevices.begin(), PhysicalDevices.end(), GPUs.begin(),
			[](VkPhysicalDevice PhysicalDevice) -> VulkanGPU
			{ return VulkanGPU{ PhysicalDevice }; });

		return GPUs;
	}


} } // namespace VE::Runtime