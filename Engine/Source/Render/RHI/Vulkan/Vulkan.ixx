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

	class VulkanContext
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
		VulkanContext() noexcept = default;

		void Bootstrap()
		{
			VulkanLoader::Bootstrap();

			CreateVulkanInstance();
		}
		
		void Terminate()
		{
			DestroyVulkanInstance();

			VulkanLoader::Terminate();
		}

		void CreateVulkanInstance()
		{
			// Layers
			Array<const char*> EnabledLayers
			{
				"VK_LAYER_KHRONOS_validation",
				"VK_LAYER_RENDERDOC_Capture"
			};
			Instance.Layers.resize(EnabledLayers.size());
			for (Int32 i = 0; i < Instance.Layers.size(); ++i)
			{ Instance.Layers[i] = EnabledLayers[i]; }

			// Extensions
			Array<const char*> EnabledExtensions
			{
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			};
			Instance.Extensions.resize(EnabledExtensions.size());
			for (Int32 i = 0; i < Instance.Extensions.size(); ++i)
			{ Instance.Extensions[i] = EnabledExtensions[i]; }

			UInt32 instance_layer_property_count = 0;
			VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_property_count, nullptr));

			Array<VkLayerProperties> layer_properties(instance_layer_property_count);
			vkEnumerateInstanceLayerProperties(&instance_layer_property_count, layer_properties.data());
			
			Array<String> AvailaleLayers;
			auto res = layer_properties | std::ranges::views::transform(
				[](const VkLayerProperties& properties)
				{ return properties.layerName; });

			/*std::transform(layer_properties.begin(), layer_properties.end(),
				std::back_inserter(AvailaleLayers),
				[](const VkLayerProperties& properties)
				{ return properties.layerName; });*/

			/*String LayerNames;
			for (const auto& Str : res) LayerNames += str + String("\n\n");
			Log::Info(LayerNames);*/
		}

		void DestroyVulkanInstance()
		{

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