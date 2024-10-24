module;
#include <ViseraEngine>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk.h>
#include <GLFW/glfw3.h>

export module Visera.Render.RHI.Vulkan;
import :Loader;

import Visera.Core.Log;

export namespace VE
{
	#define VK_CHECK(Func) { if (VK_SUCCESS != Func) Assert(False); }

	class RHI;

	class Vulkan
	{
		friend class RHI;
	public:
		/* << Vulkan Objects >>
		   1. Instance
		   2. Messenger
		*/
		struct {
			const char*					AppName		= "Visera";
			uint32_t					AppVersion	= VK_MAKE_VERSION(1, 0, 0);
			VkInstance					Handle		{ VK_NULL_HANDLE };	
			
			Array<const char*>			Layers;
			Array<const char*>			Extensions;
			operator VkInstance() const { return Handle; }
		}Instance;

		struct {
			VkDebugUtilsMessengerEXT			 Handle;
			PFN_vkDebugUtilsMessengerCallbackEXT Callback = DefaultMessengerCallback;
			operator VkDebugUtilsMessengerEXT() const { return Handle; }
		}Messenger;

		struct
		{
			GLFWwindow*				Handle = nullptr;
			operator GLFWwindow* () const { return Handle; }
		}Window;

		struct {
			VkSurfaceKHR                        Handle{ VK_NULL_HANDLE };
			std::vector<VkSurfaceFormatKHR>		Formats;
			std::vector<VkPresentModeKHR>		PresentModes;
			operator VkSurfaceKHR() const { return Handle; }
		}Surface{};

		const VkAllocationCallbacks* AllocationCallbacks  { nullptr };

	private:
		Vulkan() noexcept = default;

		void Bootstrap()
		{
			VulkanLoader::Bootstrap();

			CreateVulkanInstance();
		}

		void CreateVulkanInstance()
		{
			// Layers
			Array<const char*> enabled_layers
			{
				"VK_LAYER_KHRONOS_validation",
				"VK_LAYER_RENDERDOC_Capture"
			};
			Instance.Layers.resize(enabled_layers.size());
			for (Int32 i = 0; i < Instance.Layers.size(); ++i)
			{ Instance.Layers[i] = enabled_layers[i]; }

			// Extensions
			Array<const char*> enabled_extensions
			{
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			};
			Instance.Extensions.resize(enabled_extensions.size());
			for (Int32 i = 0; i < Instance.Extensions.size(); ++i)
			{ Instance.Extensions[i] = enabled_extensions[i]; }

			UInt32 instance_layer_property_count = 0;
			vkEnumerateInstanceLayerProperties(&instance_layer_property_count, nullptr);
			//VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_property_count, nullptr));

			/*Array<VkLayerProperties> layer_properties(instance_layer_property_count);
			vkEnumerateInstanceLayerProperties(&instance_layer_property_count, layer_properties.data());
			*/
			//Array<String> available_layers;
			/*std::transform(layer_properties.begin(), layer_properties.end(),
				std::back_inserter(available_layers),
				[](const VkLayerProperties& properties)
				{ return properties.layerName; });

			for (const auto& str : available_layers)
				Log::Info(str);*/
		}

		void Terminate()
		{

			VulkanLoader::Terminate();
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL
			DefaultMessengerCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData)
		{
			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				Log::Debug("[Vulkan]: {}",	pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				Log::Warn("[Vulkan]: {}",	pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				Log::Fatal("[Vulkan]: {}",	pCallbackData->pMessage);
				break;
			default:
				Log::Error("[Vulkan]: Unknow Message Severity {}", ErrorCode(messageSeverity));
			}
			return VK_FALSE; // Always return VK_FALSE
		}
	};

} // namespace VE