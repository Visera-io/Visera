from __MODULE__ import *

from os import path

def Generate(argv) -> int:
    """[debug/release]"""
    if len(argv) < 2: raise RuntimeError(f"generate {Generate.__doc__}")
    
    app_name = argv[0]
    prj_preset = f"{VISERA_OS_TYPE.lower()}-{argv[1].lower()}"
    Log.Info(f"Generating {prj_preset}...")
    app_path = path.join(VISERA_APP_PATH, app_name)
    if not path.isdir(app_path):
        raise RuntimeError(f"Failed to find {app_path}!")
    
    return subprocess.run(
        args = [
                "cmake",
                "-S", VISERA_ROOT_PATH,
                "-B", path.join(app_path, "Project"),
                "-D", f"APP_NAME={app_name}",
                "--preset", prj_preset,
                ],
        cwd  = VISERA_ROOT_PATH).returncode