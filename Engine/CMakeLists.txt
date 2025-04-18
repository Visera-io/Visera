cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set(VISERA_ENGINE_DIR           "${CMAKE_CURRENT_SOURCE_DIR}")
set(VISERA_ENGINE_CONFIGS_DIR   "${VISERA_ENGINE_DIR}/Configs")
set(VISERA_ENGINE_PACKAGE_DIR   "${VISERA_ENGINE_DIR}/Packages")
set(VISERA_ENGINE_SOURCE_DIR    "${VISERA_ENGINE_DIR}/Source")
set(VISERA_ENGINE_TEMPLATES_DIR "${VISERA_ENGINE_DIR}/Templates")

set(VISERA_ENGINE_ASSETS_DIR    "${VISERA_ENGINE_DIR}/Assets")
set(VISERA_ENGINE_SHADERS_DIR   "${VISERA_ENGINE_ASSETS_DIR}/Shaders")
set(VISERA_ENGINE_IMAGES_DIR     "${VISERA_ENGINE_ASSETS_DIR}/Images")
set(VISERA_ENGINE_MODELS_DIR     "${VISERA_ENGINE_ASSETS_DIR}/Models")
set(VISERA_ENGINE_FONTS_DIR     "${VISERA_ENGINE_ASSETS_DIR}/Fonts")

#[Engine]
file(GLOB_RECURSE VISERA_ENGINE_MODULES     "${VISERA_ENGINE_SOURCE_DIR}/*.ixx")
file(GLOB_RECURSE VISERA_ENGINE_HEADERS     "${VISERA_ENGINE_SOURCE_DIR}/*.h")
file(GLOB_RECURSE VISERA_ENGINE_CONFIGS     "${VISERA_ENGINE_CONFIGS_DIR}/*")
file(GLOB_RECURSE VISERA_ENGINE_SHADERS     "${VISERA_ENGINE_SHADERS_DIR}/*.slang")
file(GLOB_RECURSE VISERA_ENGINE_TEMPLATES   "${VISERA_ENGINE_TEMPLATES_DIR}/*")

#[TODO]: Move Assets to binary dir and read according to Config
add_compile_definitions(VISERA_ENGINE_ASSETS_DIR=\"${VISERA_ENGINE_ASSETS_DIR}\")
add_compile_definitions(VISERA_ENGINE_SHADERS_DIR=\"${VISERA_ENGINE_SHADERS_DIR}\")
add_compile_definitions(VISERA_ENGINE_IMAGES_DIR=\"${VISERA_ENGINE_IMAGES_DIR}\")
add_compile_definitions(VISERA_ENGINE_MODELS_DIR=\"${VISERA_ENGINE_MODELS_DIR}\")
add_compile_definitions(VISERA_ENGINE_FONTS_DIR=\"${VISERA_ENGINE_FONTS_DIR}\")

add_library(${VISERA})

target_sources(${VISERA}
                PUBLIC
                FILE_SET "visera_engine_modules" TYPE CXX_MODULES
                FILES ${VISERA_ENGINE_MODULES})

target_sources(${VISERA}
                PRIVATE
                ${VISERA_ENGINE_CONFIGS}
                ${VISERA_ENGINE_HEADERS}
                ${VISERA_ENGINE_SHADERS}
                "TODO.md")

target_include_directories(${VISERA} PUBLIC ${VISERA_ENGINE_CONFIGS_DIR})

#<< Install/Download Packages >>
include(install_vulkan)
link_vulkan(${VISERA})

include(install_volk)
link_volk(${VISERA})

include(install_vma)
link_vma(${VISERA})

include(install_glfw)
link_glfw(${VISERA})

include(install_slang)
link_slang(${VISERA})

include(install_imgui)
link_imgui(${VISERA})

include(install_tbb)
link_tbb(${VISERA})

include(install_embree)
link_embree(${VISERA})

include(install_assimp)
link_assimp(${VISERA})

include(install_freeimage)
link_freeimage(${VISERA})

include(install_stb)
link_stb(${VISERA})

include(install_libpng)
link_libpng(${VISERA})

include(install_spdlog)
link_spdlog(${VISERA})

include(install_eigen)
link_eigen(${VISERA})

include(install_rapidjson)
link_rapidjson(${VISERA})

#target_link_libraries(${VISERA} PUBLIC Visera-Packages)

source_group(TREE ${VISERA_ENGINE_DIR}
    FILES
    ${VISERA_ENGINE_CONFIGS}
    ${VISERA_ENGINE_MODULES}
    ${VISERA_ENGINE_SHADERS}
    ${VISERA_ENGINE_TEMPLATES})

#[Project]
set_target_properties(${VISERA} PROPERTIES FOLDER "Visera")