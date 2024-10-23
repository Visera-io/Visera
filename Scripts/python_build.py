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
    
    return rc