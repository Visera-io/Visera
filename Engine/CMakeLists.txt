cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

#[Engine]
file(GLOB_RECURSE VISERA_ENGINE_MODULES     "${VISERA_ENGINE_SOURCE_DIR}/*.ixx")
file(GLOB_RECURSE VISERA_ENGINE_HEADERS     "${VISERA_ENGINE_SOURCE_DIR}/*.h")
file(GLOB_RECURSE VISERA_ENGINE_CONFIGS     "${VISERA_ENGINE_CONFIGS_DIR}/*")
file(GLOB_RECURSE VISERA_ENGINE_SHADERS     "${VISERA_ENGINE_SHADERS_DIR}/*.slang")
file(GLOB_RECURSE VISERA_ENGINE_TEMPLATES   "${VISERA_ENGINE_TEMPLATES_DIR}/*")

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

include(install_openexr)
link_openexr(${VISERA})

if(WIN32)
include(install_spdlog_old)
link_spdlog(${VISERA})
else()
include(install_spdlog)
link_spdlog(${VISERA})
endif()

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