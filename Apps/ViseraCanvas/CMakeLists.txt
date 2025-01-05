
cmake_minimum_required(VERSION 3.25.0 FATAL_ERROR)
project("ViseraCanvas"
    LANGUAGES CXX
    VERSION "0.0.0"
    HOMEPAGE_URL ""
)
set(APP_NAME ViseraCanvas)
# [Environment]
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF)       # Vendor-specific Extensions
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    "${PROJECT_SOURCE_DIR}/Binaries/Debug/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/Binaries/Release/")

file(GLOB_RECURSE APP_MODULE_INTERFACE   "${PROJECT_SOURCE_DIR}/Source/*.ixx")
file(GLOB_RECURSE APP_MODULE_IMPLEMENT   "${PROJECT_SOURCE_DIR}/Source/*.cpp")

add_executable(${APP_NAME}
            ${APP_MODULE_INTERFACE}
            ${APP_MODULE_IMPLEMENT})

#[Link to Engine]
target_link_libraries(
            ${APP_NAME}
            PRIVATE
            ${VISERA})
            
source_group(TREE ${PROJECT_SOURCE_DIR}
            FILES
            ${APP_MODULE_INTERFACE}
            ${APP_MODULE_IMPLEMENT})
#[IDE::Visual Studio]
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

#[Install]
#install(TARGETS ${VISERA} RUNTIME DESTINATION ${VISERA_BINARY_DIR})