#pragma once
#include <Visera.h>
#define VK_NO_PROTOTYPES
#include <volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#if (VE_IS_APPLE_SYSTEM)
#include <vk_mem_alloc.h>
#else //if defined(VE_ON_X86_CPU)
#include <vma/vk_mem_alloc.h>
#endif