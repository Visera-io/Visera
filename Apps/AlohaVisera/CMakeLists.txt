
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

file(GLOB_RECURSE VISERA_APP_MODULES "${PROJECT_SOURCE_DIR}/Source/*.ixx")

add_executable(${VISERA_APP})

target_sources(${VISERA_APP}
               PUBLIC
               FILE_SET "visera_app_modules" TYPE CXX_MODULES
               FILES ${VISERA_APP_MODULES})

#[Link to Engine]
target_link_libraries(
            ${VISERA_APP}
            PRIVATE
            ${VISERA})

#[FIXME]:"tbb12_debug.dll" conflicts with embree's dependency name "tbb12.dll"
if(WIN32)
add_custom_command(
    TARGET ${VISERA_APP}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VISERA_APP_DEBUG_DIR}/tbb12_debug.dll"
        "$<TARGET_FILE_DIR:${VISERA_APP}>/tbb12.dll"
)
endif()
            
source_group(TREE ${PROJECT_SOURCE_DIR}
            FILES
            ${VISERA_APP_MODULES})

#[IDE::Visual Studio]
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VISERA_APP})

# [Install]
# install(TARGETS ${VISERA} RUNTIME DESTINATION ${VISERA_BINARY_DIR})