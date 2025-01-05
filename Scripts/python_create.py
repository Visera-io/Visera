from python import *

from os import makedirs

def Create(argv) -> int:
    """<app_name> <app_author>"""
    if len(argv) < 2: raise RuntimeError(f"create {Create.__doc__}")
    
    app_name = argv[0]
    app_author = argv[1]
    Log.Info(f"Creating {app_name} (Author: {app_author})...")
    
    app_path = path.join(VISERA_APP_PATH, app_name)
    
    if path.isdir(app_path):
        raise RuntimeError(f"Failed to create a new app because {app_name} has already existed!")

    makedirs(path.join(app_path, "Assets"), exist_ok=True)
    makedirs(path.join(app_path, "Binaries"), exist_ok=True)
    makedirs(path.join(app_path, "Binaries", "Debug"), exist_ok=True)
    makedirs(path.join(app_path, "Binaries", "Release"), exist_ok=True)
    makedirs(path.join(app_path, "Extern"), exist_ok=True)
    makedirs(path.join(app_path, "Project"), exist_ok=True)
    makedirs(path.join(app_path, "Source"), exist_ok=True)
    
    with open(path.join(app_path, "CMakeLists.txt"), 'w') as CMakeLists:
        CMakeLists.write(f'''
cmake_minimum_required(VERSION 3.25.0 FATAL_ERROR)
project("{app_name}"
    LANGUAGES CXX
    VERSION "0.0.0"
    HOMEPAGE_URL ""
)
set(APP_NAME {app_name})''')
        
        CMakeLists.write('''
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
#install(TARGETS ${VISERA} RUNTIME DESTINATION ${VISERA_BINARY_DIR})''')
        
        with open(path.join(app_path, "Source", "Launcher.cpp"), 'w') as AppLauncher:
            AppLauncher.write(f"import Visera;\nimport {app_name};\nusing namespace VISERA_APP_NAMESPACE;\n\nint main(int argc, char* argv[])")
            AppLauncher.write("\n{")
            AppLauncher.write("\tVE::Visera ViseraEngine( new App );\n\treturn ViseraEngine.Run();")
            AppLauncher.write("\n}")
            
        with open(path.join(app_path, "Source", f"{app_name}.ixx"), 'w') as App:
            App.write(f"module;\n#include <Visera.h>\nexport module {app_name};\nimport Visera;\n")
            App.write("\nexport namespace VISERA_APP_NAMESPACE\n{")
            App.write("\n\tclass App final : public VE::ViseraApp")
            App.write("\n\t{\n\tpublic:\n\t\tvirtual void Tick() override\n\t\t{\n\n\t\t}\n")
            App.write("\n\t\tvirtual void RenderTick() override\n\t\t{\n\n\t\t}\n")
            App.write("\n\t\tvirtual void Bootstrap() override\n\t\t{\n\n\t\t}\n")
            App.write("\n\t\tvirtual void Terminate() override\n\t\t{\n\n\t\t}\n")
            
            App.write("\t};\n} // namespace VISERA_APP_NAMESPACE")
    
    return VISERA_SUCCESS