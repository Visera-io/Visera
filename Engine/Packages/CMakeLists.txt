add_library(VISERA_ENGINE_EXTERN INTERFACE)

#[Manager]
add_custom_target(Visera-Packages ALL) # For adding denpendencies
#set_target_properties(Visera-Packages PROPERTIES FOLDER "${PROJECT_NAME}")

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

# Vulkan
if (NOT TARGET Vulkan::Vulkan)
    message(STATUS "\nLoading Vulkan (Vulkan::Vulkan)")
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

# # DXC
# if (NOT TARGET Microsoft::DirectXShaderCompiler)
#     message(STATUS "\nLoading DXC (Microsoft::DirectXShaderCompiler)")
#     FIND_PACKAGE(directx-dxc CONFIG REQUIRED)
#     target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE Microsoft::DirectXShaderCompiler)
#     add_custom_command(
#         TARGET Visera-Packages
#         POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<TARGET_FILE:Microsoft::DirectXShaderCompiler>
#         $<TARGET_FILE_DIR:${APP_NAME}>
#     )
#     # << DXIL used in DX12 API : https://docs.vulkan.org/guide/latest/hlsl.html>>
#     # target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE Microsoft::DXIL)
#     # add_custom_command(
#     #     TARGET Visera-Packages
#     #     POST_BUILD
#     #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     #     $<TARGET_FILE:Microsoft::DXIL>
#     #     $<TARGET_FILE_DIR:${APP_NAME}>
#     # )
# endif()