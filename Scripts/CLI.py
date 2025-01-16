from __MODULE__ import *
from CLI_create   import Create
from CLI_generate import Generate
from CLI_build    import Build
from CLI_setup    import Setup
from CLI_run      import Run

class ViseraCLI:
    def __init__(self):
        self.Workspace  = "Visera"
        self.WorkDir    = VISERA_ENGINE_PATH
        self.History    = []
        self.MaxHistory = 10
        self.bSaveHistory= False
        self.CommandSets= {
            "Visera" :{"setup":Setup, "create":Create, "open":self.Open, "help":self.Help, "exit":self.Exit},
            "App"    :{"generate":Generate, "build":Build, "run":Run, "help":self.Help, "exit":self.Exit}
        }
        
    def Listen(self) -> bool:
        if len(self.History) > self.MaxHistory: self.History = self.History[:self.MaxHistory]
        
        newinput = input(f"[{self.Workspace}]> ")
        if newinput != "":
            if newinput == "!!": #Linux execute the last command
                if len(self.History) > 0: newinput = self.History[-1]
                else: newinput = ""
            else: self.History.append(newinput)

            newinput = newinput.split()
            command     = newinput[0]
            arguments   = []
            if len(newinput)  > 1 : arguments = newinput[1:]
            
            #'At' Grammar (Path Find)
            #[TODO]: Path Family (Optimizing Search Efficiency)
            if command[0] == '@':
                target = command[1:]
                if target != "":
                    if '.' not in target and VISERA_OS_TYPE == "Windows": target += ".exe"
                    for path_family, paths in VISERA_ENV_PATH.items():
                        for _path in paths:
                            target = path.join(_path, target)
                            if path.isfile(target):
                                arguments = [target] + arguments
                                rc = subprocess.run(
                                    args = arguments,
                                    shell= True,
                                    cwd  = self.WorkDir).returncode
                                return True
                    Log.Error(f"Failed to find {target} in VISERA_ENV_PATH!")
                    return True
                else:
                    print(VISERA_ENV_PATH) #Help
                    return True
            
            commandset = None
            if self.Workspace == "Visera": commandset = self.CommandSets["Visera"]
            else:
                commandset = self.CommandSets["App"]
                arguments = [self.Workspace[6+1:]] + arguments
            
            syscall = commandset.get(command)
            try:
                if not syscall:
                    Log.Error(f"Failed to find the command \"{command}\"")
                elif VISERA_FAILED == syscall(arguments):
                    Log.Error(f"Failed to execute the command \"{command}\"")
            except RuntimeError as ErrorMessage:
                Log.Error(f"{ErrorMessage}")
        else:
            pass
        
        return True
    
    def Open(self, arguments):
        """open <app_name>"""
        if len(arguments) < 1: Log.Error(f"generate {Generate.__doc__}")
        app_name = arguments[0]
        app_path = path.join(VISERA_APP_PATH, app_name)
        if not path.isdir(app_path):
            Log.Error(f"Failed to find {app_path}!")
        else:
            self.Workspace += f"|{app_name}"
            self.WorkDir    = app_path
    
    def Help(self, arguments):
        """Get the command list in current workspace."""
        prompt = ""
        if self.Workspace == "Visera":
            for alias, cmd in self.CommandSets["Visera"].items():
                prompt += f"- {alias}".ljust(20) + f"{cmd.__doc__}\n"
        else:
            for alias, cmd in self.CommandSets["App"].items():
                prompt += f"- {alias}".ljust(20) + f"{cmd.__doc__}\n"
        Log.Info(prompt)
    
    def Exit(self, arguments):
        """Exit current workspace (add '!' to exit all)."""
        if arguments and arguments[-1] == "!": exit(0)
        if self.Workspace == "Visera":
            exit(0)
        else:
            self.Workspace = "Visera"
            self.WorkDir   = VISERA_ENGINE_PATH
            return VISERA_SUCCESS

if "__main__" == __name__:
    Log.Info("Welcome to use Visera Engine!")
    
    CLI = ViseraCLI()
    
    while CLI.Listen():pass