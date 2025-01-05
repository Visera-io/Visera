
from python import *
import python_create
import python_generate
import python_build
import python_setup
import python_run

#Global Registry
registry = {}
registry["setup"]    = python_setup.Setup
registry["create"]    = python_create.Create
registry["generate"] = python_generate.Generate
registry["build"] = python_build.Build
registry["run"] = python_run.Run

def Help():
    prompt = "Usage: Visera <Command> [Arguments]\n\n"\
             "<Table of Commands>"
    for name, func in registry.items():
        prompt += f"\n - {name}:\t{func.__doc__}"
    Log.Info(f"{prompt}")

if "__main__" == __name__:
    if len(argv) < 2:
        Help()
        exit(VISERA_FAILED)
        
    try:
        # Parse Arguments     
        command   = argv[1]
        arguments = argv[2:]
        
        # Parse Command
        func = registry.get(command)
        if not func:
            raise RuntimeError(f"Failed to find the command \"{command}\"")
        if VISERA_FAILED == func(arguments):
            raise RuntimeError(f"Failed to execute the command \"{command}\"")
        
    except RuntimeError as error:
        Log.Error(f"{error}")
        exit(VISERA_FAILED)