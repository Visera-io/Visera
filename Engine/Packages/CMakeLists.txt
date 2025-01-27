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

# # ICU
# if (NOT TARGET ICU)
#     message(STATUS "\nLoading ICU (ICU::uc, ICU::i18n)")
#     find_package(ICU REQUIRED COMPONENTS uc i18n)
#     #target_include_directories(VISERA_ENGINE_EXTERN INTERFACE ${ICU_INCLUDE_DIRS})
#     target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE ICU::uc ICU::i18n)
#     add_custom_command(
#         TARGET Visera-Packages
#         POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<TARGET_FILE:ICU::i18n>
#         $<TARGET_FILE_DIR:${APP_NAME}>
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<TARGET_FILE:ICU::uc>
#         $<TARGET_FILE_DIR:${APP_NAME}>
#     )
# endif()

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