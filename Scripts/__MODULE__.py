from sys import argv
from os import path, environ, access
from platform import system
from datetime import datetime
import subprocess

VISERA_VERSION      =   "1.0.0"
VISERA_ENGINE_NAME  =   f"Visera {VISERA_VERSION}"
VISERA_ROOT_PATH    =   path.dirname((path.dirname(path.abspath(__file__))))
VISERA_APP_PATH     =   path.join(VISERA_ROOT_PATH, "Apps")
VISERA_PYTHON_PATH  =   path.join(VISERA_ROOT_PATH, "Python")
VISERA_ENGINE_PATH  =   path.join(VISERA_ROOT_PATH, "Engine")
VISERA_PACKAGES_PATH  =   path.join(VISERA_ROOT_PATH, "Packages")

VISERA_OS_TYPE      =   system()
VISERA_ON_WINDOWS   =   VISERA_OS_TYPE == "Windows"
VISERA_ON_MACOS     =   VISERA_OS_TYPE == "Darwin"

VISERA_ENV_PATH     =   {
                            "Vulkan" : [],#[path.join(environ["VULKAN_SDK"], "bin")], #vulkanCapsViewer, vkconfig, vkcube [TODO]:Alias
                            "Vcpkg"  : [path.join(VISERA_PACKAGES_PATH, "Vcpkg")]
                        }
#if VISERA_ON_MACOS: VISERA_ENV_PATH["Vulkan"].append(path.join(environ["VULKAN_SDK_DIR"], "Applications"))
if VISERA_ON_WINDOWS: VISERA_ENV_PATH["Vulkan"].append(path.join(environ["VULKAN_SDK"], "bin"))

VISERA_SUCCESS      =   0
VISERA_FAILED       =   not VISERA_SUCCESS

class Logger:
    LOG_LEVEL_TRIVIAL_BIT   = 0b000001
    LOG_LEVEL_INFO_BIT      = 0b000010
    LOG_LEVEL_DEBUG_BIT     = 0b000100
    LOG_LEVEL_WARN_BIT      = 0b001000
    LOG_LEVEL_ERROR_BIT     = 0b010000
    LOG_LEVEL_ALL_BIT       = 0b111111
    def __init__(self, level = LOG_LEVEL_ALL_BIT ^ LOG_LEVEL_TRIVIAL_BIT ^ LOG_LEVEL_DEBUG_BIT):
        self.level = level
        #Config
        self.config_color_debug     = "90m" # Gray
        self.config_color_error     = "91m" # Red
        self.config_color_info      = "92m" # Green
        self.config_color_warn      = "93m" # Yellow
        self.config_color_prompt    = "94m" # Blue
        self.config_color_title     = "95m" # Purple

    def Trivial(self, message):
        if not (Logger.LOG_LEVEL_TRIVIAL_BIT & self.level): return
        print(f"{self.TitleBar()}\033[{self.config_color_debug}{message}\033[0m")

    def Info(self, message):
        if not (Logger.LOG_LEVEL_INFO_BIT & self.level): return
        print(f"{self.TitleBar()}\033[{self.config_color_info}{message}\033[0m")
        
    def Debug(self, message):
        if not (Logger.LOG_LEVEL_DEBUG_BIT & self.level): return
        print(f"{self.TitleBar()}\033[{self.config_color_prompt}{message}\033[0m")
        
    def Warn(self, message):
        if not (Logger.LOG_LEVEL_WARN_BIT & self.level): return
        print(f"{self.TitleBar()}\033[{self.config_color_warn}{message}\033[0m")
        
    def Error(self, message):
        if not (Logger.LOG_LEVEL_ERROR_BIT & self.level): return
        print(f"{self.TitleBar()}\033[{self.config_color_error}{message}\033[0m")

    def TitleBar(self):
        return f"\033[{self.config_color_title}[Visera {datetime.now()}]\n\033[0m"
#Global Logger
Log = Logger()