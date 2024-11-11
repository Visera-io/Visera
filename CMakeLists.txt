
cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

SET(VISERA_VERSION_MAJOR 1)
SET(VISERA_VERSION_MINOR 0)
SET(VISERA_VERSION_PATCH 0)
SET(VISERA_VERSION "${VISERA_VERSION_MAJOR}.${VISERA_VERSION_MINOR}.${VISERA_VERSION_PATCH}")

set(VISERA "Visera")
project("${APP_NAME} (${VISERA} ${VISERA_VERSION})"
    VERSION         "${VISERA_VERSION}"
    HOMEPAGE_URL    "www.visera.io"
    LANGUAGES       CXX
)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS        OFF) # Vendor-specific Extensions

#[Properties]
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#[Project]
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR
      "\n"
      "In-source builds are not allowed.\n"
      "Instead, provide a path to build tree like so:\n"
      "cmake -B <destination>\n"
      "\n"
      "To remove files you accidentally created execute:\n"
      "rm -rf CMakeFiles CMakeCache.txt\n"
    )
endif()

#[Environment]
set(VISERA_DIR          "${PROJECT_SOURCE_DIR}")
set(VISERA_APP_DIR      "${VISERA_DIR}/Apps")
set(VISERA_CACHE_DIR    "${VISERA_DIR}/Cache")
set(VISERA_ENGINE_DIR   "${VISERA_DIR}/Engine")
set(VISERA_SCRIPTS_DIR  "${VISERA_DIR}/Scripts")

#[Macros]
add_compile_definitions(NOMINMAX)
add_compile_definitions(VISERA_ENGINE_NAME=\"${VISERA}\")
add_compile_definitions(VISERA_APP_NAME=\"${APP_NAME}\")
add_compile_definitions(VISERA_APP_NAMESPACE=${APP_NAME})
add_compile_definitions(VISERA_APP_ASSETS_DIR=\"${VISERA_APP_DIR}/${APP_NAME}/Assets\")
#[Engine]
add_subdirectory("${VISERA_ENGINE_DIR}")
#[App]
add_subdirectory("${VISERA_APP_DIR}/${APP_NAME}")