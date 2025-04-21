
cmake_minimum_required(VERSION 3.25.0 FATAL_ERROR)

project(${VISERA_APP}
    LANGUAGES CXX
    VERSION "0.0.0"
    HOMEPAGE_URL ""
)

# [Environment]
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF)       # Vendor-specific Extensions

file(GLOB_RECURSE APP_MODULE_INTERFACE   "${PROJECT_SOURCE_DIR}/Source/*.ixx")
file(GLOB_RECURSE APP_MODULE_IMPLEMENT   "${PROJECT_SOURCE_DIR}/Source/*.cpp")

add_executable(${VISERA_APP}
               ${APP_MODULE_INTERFACE}
               ${APP_MODULE_IMPLEMENT})

#[Link to Engine]
target_link_libraries(
            ${VISERA_APP}
            PRIVATE
            ${VISERA})

#[FIXME]:"tbb12_debug.dll" conflicts with embree's dependency name "tbb12.dll"
add_custom_command(
    TARGET ${VISERA_APP}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VISERA_APP_DEBUG_DIR}/tbb12_debug.dll"
        "$<TARGET_FILE_DIR:${VISERA_APP}>/tbb12.dll"
)

            
source_group(TREE ${PROJECT_SOURCE_DIR}
            FILES
            ${APP_MODULE_INTERFACE}
            ${APP_MODULE_IMPLEMENT})

#[IDE::Visual Studio]
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VISERA_APP})

# [Install]
# install(TARGETS ${VISERA} RUNTIME DESTINATION ${VISERA_BINARY_DIR})