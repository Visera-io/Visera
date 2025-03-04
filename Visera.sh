#!/bin/zsh
VULKAN_SDK_DIR=$(find /Users/$USER/VulkanSDK -type d -name "1.*.*.*" | sort -V | tail -n 1)
export VULKAN_SDK_DIR
source $VULKAN_SDK_DIR/setup-env.sh
python Scripts/CLI.py