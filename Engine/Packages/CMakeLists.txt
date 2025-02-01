add_library(VISERA_ENGINE_EXTERN INTERFACE)

#[Manager]
add_custom_target(Visera-Packages ALL) # For adding denpendencies
set_target_properties(Visera-Packages PROPERTIES FOLDER "Visera/Rules")

# Vulkan
if (NOT TARGET Vulkan::Vulkan)
    message(STATUS "\nLoading Vulkan (Vulkan::Vulkan)")
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "Failed to find VulkanSDK on your system!")
    else()
        # Extract the version from the path
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" VULKAN_VERSION "$ENV{VULKAN_SDK}")
        message("Found VulkanSDK ${VULKAN_VERSION} on current system.")
        if(VULKAN_VERSION)
            # Compare with required version (1.4.0)
            if(VULKAN_VERSION VERSION_LESS "1.4.0")
                message(FATAL_ERROR "VulkanSDK version is required: >=1.4.0, Found: ${VULKAN_VERSION}")
            endif()
        else()
            message(WARNING "Could not determine Vulkan SDK version.")
        endif()
    endif()
    FIND_PACKAGE(Vulkan REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE Vulkan::Vulkan)
endif()

# Volk
if (NOT TARGET volk::volk)
    message(STATUS "\nLoading Volk (volk::volk_headers)")
    FIND_PACKAGE(volk CONFIG REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE volk::volk_headers)
endif()

#VMA
if (NOT TARGET GPUOpen::VulkanMemoryAllocator)
    message(STATUS "\nLoading VMA (GPUOpen::VulkanMemoryAllocator)")
    FIND_PACKAGE(VulkanMemoryAllocator CONFIG REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE GPUOpen::VulkanMemoryAllocator)
endif()

# Slang
if (NOT TARGET slang)
    message(STATUS "\nLoading Slang (slang)")
    # Make sure that Version(VulkanSDK) >= "1.4.0"
    add_library(slang UNKNOWN IMPORTED)
    set_target_properties(slang
        PROPERTIES
        IMPORTED_LOCATION "$ENV{VULKAN_SDK}/Lib/slang.lib"
        INTERFACE_INCLUDE_DIRECTORIES "$ENV{VULKAN_SDK}/Include/slang")
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE slang)
endif()

# Spdlog
if (NOT TARGET spdlog)
    message(STATUS "\nLoading Spdlog (spdlog::spdlog)")
    FIND_PACKAGE(spdlog REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE spdlog::spdlog)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:spdlog::spdlog>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:fmt::fmt>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Eigen
if (NOT TARGET Eigen3::Eigen)
    message(STATUS "\nLoading Eigen3 (Eigen3::Eigen)")
    FIND_PACKAGE(Eigen3 REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE Eigen3::Eigen)
endif()

# GLFW3
if (NOT TARGET glfw)
    message(STATUS "\nLoading GLFW3 (glfw)")
    find_package(glfw3 REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE glfw)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Dear ImGui
if (NOT TARGET imgui::imgui)
    message(STATUS "\nLoading Dear ImGui-Docking (imgui::imgui)")
    find_package(imgui REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE imgui::imgui)
endif()

# Intel OneAPI TBB
if (NOT TARGET TBB::tbb)
    message(STATUS "\nLoading Intel OneAPI TBB (TBB::tbb)")
    find_package(TBB REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE TBB::tbb)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE TBB::tbbmalloc)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:TBB::tbb>
        $<TARGET_FILE_DIR:${APP_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:TBB::tbbmalloc>
        $<TARGET_FILE_DIR:${APP_NAME}>
        #"tbb12_debug.dll" conflicts with embree's dependency name "tbb12.dll"
        COMMAND ${CMAKE_COMMAND} -E rename
        $<TARGET_FILE_DIR:${APP_NAME}>/$<TARGET_FILE_NAME:TBB::tbb>
        $<TARGET_FILE_DIR:${APP_NAME}>/tbb12.dll
    )
endif()

# Intel Embree4
if (NOT TARGET embree)
    message(STATUS "\nLoading Intel Embree4 (embree4)")
    find_package(embree REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE embree)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:embree>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Assimp
if (NOT TARGET assimp)
    message(STATUS "\nLoading Assimp (assimp)")
    find_package(assimp CONFIG REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE assimp::assimp)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp::assimp>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Free Image
if (NOT TARGET freeimage)
    message(STATUS "\nLoading FreeImage (freeimage::FreeImage)")
    find_package(freeimage CONFIG REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE
            freeimage::FreeImage
            freeimage::FreeImagePlus)
    add_custom_command(
        TARGET Visera-Packages
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:freeimage::FreeImage>
        $<TARGET_FILE_DIR:${APP_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:freeimage::FreeImagePlus>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Rapid JSON
if (NOT TARGET rapidjson)
    message(STATUS "\nLoading Rapid JSON (rapidjson)")
    find_package(RapidJSON CONFIG REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE rapidjson)
endif()