# LBR

Guide on setting up the project with Docker.

## Prerequisites
- WSL 2
- Git (Link your GitHub account)
- Docker Desktop
- Visual Studio Code + Dev Containers Extension

## Required Project Files
**`.vscode/settings.json`**
```json
  {
    "C_Cpp.intelliSenseEngine": "disabled",
    "C_Cpp.errorSquiggles": "disabled",
    "C_Cpp.autocomplete": "disabled",
    "C_Cpp.codeAnalysis.exclude": {
      "**": true
    },
    "clangd.enable": true,
    "clangd.path": "/usr/bin/clangd",
    "clangd.arguments": [
      "--compile-commands-dir=${workspaceFolder}/build/stm32h723",
      "--query-driver=/usr/bin/*",
      "--clang-tidy",
      "--header-insertion=never"
    ],
    "cmake.cmakePath": "/usr/bin/cmake",
    "cortex-debug.openocdPath": "/usr/bin/openocd",
    "cortex-debug.armToolchainPath": "/usr/bin",
    "cortex-debug.gdbPath": "/usr/bin/gdb-multiarch",
  }
```

**`.clangd`**
```.clangd
  CompileFlags: 

    CompilationDatabase: build/stm32h723 

    Add: 

      - -std=c++20 

      - --target=arm-none-eabi 

      - -mcpu=cortex-m7 

      - -mthumb 

      - -mfpu=fpv5-d16 

      - -mfloat-abi=hard 

      - -DSTM32H723xx 

      - -DSTM32H7 

      - -DUSE_HAL_DRIVER 

      - -D__cpp_lib_span=202002L 

      - -DGNUC 

      - -Darm 

      - -DARM_ARCH_7EM 

      - -Dthumb2 

  

      # Project include paths. These are relative to the workspace root. 

      - -Icommon/drivers/bus 

      - -Icommon/drivers/io 

      - -Icommon/drivers/platform/stm32h7 

      - -Icommon/drivers/time 

      - -Icommon/core/periph 

      - -Icommon/core/utils 

      - -Imcu_support/stm32/h7xx 

      - -Imcu_support/stm32/h7xx/HAL 

      - -Imcu_support/CMSIS/include 

  

  Index: 

    Background: Build 

  

  Diagnostics: 

    ClangTidy: 

      Add: 

        - readability-* 

        - performance-* 

        - bugprone-* 

      Remove: 

        - readability-magic-numbers 

        - readability-uppercase-literal-suffix 

  

  Hover: 

    ShowAKA: true 

```

## Developing
Open the project by selecting **'Dev Containers: Reopen in Container'**.
Necessary packages and toolchains should automatically install; this may take some time. 

VSCode extensions should also automatically be installed. However, some may not be enabled and you may be prompted to reload the workspace to enable them (do it).

## Building
Use the ```.sh``` script. The minimum parameters look like this: 
```./make.sh -t <name of preset>```.
For example, ```./make.sh -t stm32h723``` (see CMakePresets.json).

It's also possible to specify a target application rather than building all available apps (which is the default), by using the -a parameter: ```./make.sh -t stm32h723 -a blink```. 

Builds are by default done in Debug mode, but Release mode can be selected with the -r parameter: ```./make.sh -t stm32h723 -r```

## Debugging
First grant usb access to WSL by opening a windows terminal with admin - windows button + `x` and select Terminal (Admin). Run the following commands and take note of the ST-Link Bus ID.
````
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```

You can verify WSL has access to the ST-Link by running the command in WSL: `lsusb`

Once the usb is shared, you may have to reopen the project container.

Select 'Run and Debug' on the sidebar (left) and select a preset. Click the green play button to begin the debug session.