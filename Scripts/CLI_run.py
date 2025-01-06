from __MODULE__ import *

def Run(argv) -> int:
    """[debug/release] <arguments>"""
    if len(argv) < 2: raise RuntimeError(f"run {Run.__doc__}")
    
    app_name = argv[0]
    run_type = argv[1].lower().capitalize()
    arguments = argv[2:]
    Log.Info(f"Running {app_name} ({run_type})...")
    
    app_path = path.join(VISERA_APP_PATH, app_name, "Binaries", run_type)

    if not path.isdir(app_path):
        raise RuntimeError(f"Failed to find {app_path}!")
    
    context = subprocess.run(
                args = [app_name] + arguments,
                shell= True,
                capture_output=True,
                text=True,
                cwd  = app_path)
    print(context.stdout)
    return context.returncode