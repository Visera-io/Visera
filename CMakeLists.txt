
cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

SET(VISERA_VERSION_MAJOR 1)
SET(VISERA_VERSION_MINOR 0)
SET(VISERA_VERSION_PATCH 0)
SET(VISERA_VERSION "${VISERA_VERSION_MAJOR}.${VISERA_VERSION_MINOR}.${VISERA_VERSION_PATCH}")

set(VISERA "Visera")
project("${VISERA_APP} (${VISERA} ${VISERA_VERSION})"
    VERSION         "${VISERA_VERSION}"
    HOMEPAGE_URL    "www.visera.io"
    LANGUAGES       CXX
)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS        OFF) # Vendor-specific Extensions

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fmodules")

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" HAS_LIBCPP)

    if (HAS_LIBCPP)
        message(STATUS "Visera: Using Libc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -D_LIBCPP_VERSION")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
    else()
        message(FATAL_ERROR "libc++ is recommended in conjunction with clang. Please insteall the libc++ development headers, provided e.g. by the packages 'libc++-dev' and 'libc++abi-dev' on Debian/Ubuntu.")
    endif()
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

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
set(VISERA_DIR          "${CMAKE_CURRENT_SOURCE_DIR}")
set(VISERA_APP_DIR      "${VISERA_DIR}/Apps")
set(VISERA_PACKAGES_DIR "${VISERA_DIR}/Packages")
set(VISERA_ENGINE_DIR   "${VISERA_DIR}/Engine")
set(VISERA_CACHE_DIR    "${VISERA_DIR}/Cache")

set(VISERA_CURRENT_APP_DIR  "${VISERA_APP_DIR}/${VISERA_APP}")
set(VISERA_APP_DEBUG_DIR    "${VISERA_CURRENT_APP_DIR}/Binaries/Debug/")
set(VISERA_APP_RELEASE_DIR  "${VISERA_CURRENT_APP_DIR}/Binaries/Release/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    "${VISERA_APP_DEBUG_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${VISERA_APP_RELEASE_DIR}")

#[Macros]
add_compile_definitions(NOMINMAX)
add_compile_definitions(VISERA_DIR=\"${VISERA_DIR}\")
add_compile_definitions(VISERA_ENGINE_NAME=\"${VISERA}\")
add_compile_definitions(VISERA_APP_NAME=\"${VISERA_APP}\")
add_compile_definitions(VISERA_APP_NAMESPACE=${VISERA_APP})
add_compile_definitions(VISERA_APP_ASSETS_DIR=\"${VISERA_APP_DIR}/${VISERA_APP}/Assets\")
add_compile_definitions(VISERA_APP_SHADERS_DIR=\"${VISERA_APP_DIR}/${VISERA_APP}/Shaders\")
add_compile_definitions(VISERA_APP_CACHE_DIR=\"${VISERA_APP_DIR}/${VISERA_APP}/Cache\")
#[Packages]
add_subdirectory("${VISERA_PACKAGES_DIR}")
#[App]
add_subdirectory("${VISERA_APP_DIR}/${VISERA_APP}")
#[Engine]
add_subdirectory("${VISERA_ENGINE_DIR}")