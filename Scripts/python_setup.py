from python import *

import json
import requests
import subprocess
from os import name as os_type, path, listdir
from io import BytesIO
from zipfile import ZipFile

def Setup(argv) -> int:
    """"""
    #Update CMakePresets
    Log.Info("Updating CMakePresets.json...")
    cmake_preset_path = path.join(VISERA_ROOT_PATH, "CMakePresets.json")
    with open(cmake_preset_path, 'r') as source:
        cmake_presets = json.load(source)

    path_vcpkg  = path.join(VISERA_ENGINE_PATH,  "Packages",  "Vcpkg")
    path_manual = path.join(VISERA_ENGINE_PATH,  "Packages",  "Manual")

    vars = cmake_presets["configurePresets"][0]["cacheVariables"]
    vars["VISERA_SETUP_TIME"] = f"{datetime.now()}"

    with open(cmake_preset_path, 'w') as sink:
        json.dump(cmake_presets, sink, indent = 4)
    del cmake_presets

    #Install Vcpkg
    Log.Info("Installing Vcpkg...")
    rc = subprocess.run(
        args = [
                "bootstrap-vcpkg",
                ],
        shell= True,
        cwd  = path_vcpkg).returncode
    if VISERA_FAILED == rc: raise RuntimeError("Failed to install the Vcpkg!")
    
    #Installing Packages
    Log.Info("Installing Packages...")
    manifest_path = path.join(VISERA_ENGINE_PATH, "Packages", "Manifest.json")
    with open(manifest_path, 'r') as source:
        manifest = json.load(source)
        
    for package in manifest[VISERA_OS_TYPE]["Vcpkg"]:
        Log.Info(f"Downloading {package} via Vcpkg")
        rc = subprocess.run(
        args = [
                "vcpkg", "install", f"{package}"
                ],
        shell= True,
        cwd  = path_vcpkg).returncode
        if VISERA_FAILED == rc: raise RuntimeError(f"Failed to install the {package} via Vcpkg!")
    
    pass_manual_package = False
    if len(listdir(path_manual)) != 0:
         Log.Warn("You may have installed manual packages, would you like to reinstall? [y/n(default)]")
         if input().lower() != 'y': pass_manual_package = True
           
    if (not pass_manual_package):
        for package, url in manifest[VISERA_OS_TYPE]["Manual"].items():
            Log.Info(f"Downloading {package}")
            response = requests.get(url)
            zip_file = ZipFile(BytesIO(response.content))
            zip_file.extractall(path.join(path_manual, package))
            
    del manifest

    Log.Info("Setup Visera successfully.")
    return rc