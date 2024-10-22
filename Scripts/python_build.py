from python import *

import subprocess
from os import path, rename

def Build(argv) -> int:
    """<app_name> [debug/release]"""
    if len(argv) < 2: raise RuntimeError(f"build {Build.__doc__}")
    
    app_name = argv[0]
    build_type = argv[1].lower()
    Log.Info(f"Building {app_name} ({build_type})...")
    
    app_path = path.join(VISERA_APP_PATH, app_name)
    if not path.isdir(app_path):
        raise RuntimeError(f"Failed to find {app_path}!")
    
    rc = subprocess.run(
        args = [
                "cmake",
                "--build", path.join(app_path, "Project"),
                "--config", build_type,
                ],
        cwd  = VISERA_ROOT_PATH).returncode
    if VISERA_SUCCESS != rc:
        raise RuntimeError(f"Failed to build {app_path}!")

    if VISERA_OS_TYPE == "Windows":
        #[FIXME] Unexpected Issue...
        app_bin_path = path.join(app_path, "Binaries", build_type.capitalize())
        tbb_debug_dll = path.join(app_bin_path, "tbb12_debug.dll")
        tbb_debug_dll_new = path.join(app_bin_path, "tbb12.dll")
        if not path.exists(tbb_debug_dll_new):
            rename(tbb_debug_dll, tbb_debug_dll_new)
    else: raise RuntimeError(f"Unsupported System!")
    
    return rc