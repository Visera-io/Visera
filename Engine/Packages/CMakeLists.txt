add_library(VISERA_ENGINE_EXTERN INTERFACE)

#[Manager]
add_custom_target(VISERA_RULER ALL) # For adding denpendencies
#set_target_properties(VISERA_RULER PROPERTIES FOLDER "${PROJECT_NAME}")

# Spdlog
if (NOT TARGET spdlog)
    message(STATUS "\nLoading Spdlog (spdlog::spdlog)")
    FIND_PACKAGE(spdlog REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE spdlog::spdlog)
    add_custom_command(
        TARGET VISERA_RULER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:spdlog::spdlog>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
    add_custom_command(
        TARGET VISERA_RULER
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

# GLFW3
if (NOT TARGET glfw)
    message(STATUS "\nLoading GLFW3 (glfw)")
    find_package(glfw3 REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE glfw)
    add_custom_command(
        TARGET VISERA_RULER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()