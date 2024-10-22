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
    message(STATUS "\nLoading Eigen3 (eigen)")
    FIND_PACKAGE(Eigen3 REQUIRED)
    target_link_libraries(VISERA_ENGINE_EXTERN INTERFACE Eigen3::Eigen)
endif()