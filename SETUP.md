# LBR Setup Guide

This guide is for new team members setting up the repository with our Docker + VS Code workflow.

Start with the quick steps. The detailed config blocks are only needed if your workspace is missing them.

## 1) Quick Start

1. Install the prerequisites.
2. Clone the repo and open it in VS Code from WSL.
3. Reopen in the Dev Container.
4. Create your branch and start working.
5. Build the target you changed before pushing.

What you get:

- A reproducible development environment in a Dev Container.
- Build, debug, and serial-monitor access.

## 2) Prerequisites

### Windows host

- Install WSL 2 and Ubuntu (Ubuntu 24.04 LTS recommended):
  - https://learn.microsoft.com/windows/wsl/install
- Install usbipd-win:
  - https://learn.microsoft.com/windows/wsl/connect-usb
- Install Docker Desktop:
  - https://docs.docker.com/desktop/setup/install/windows-install/

After Docker Desktop is installed:

- Open Docker Desktop settings.
- Go to Resources -> WSL Integration.
- Enable integration for your Ubuntu distro.

### Inside WSL (Ubuntu)

- Install Git and connect your GitHub identity:

```bash
sudo apt update
sudo apt install -y git
git config --global user.name "<First Last>"
git config --global user.email "<you@example.com>"
```

- Install VS Code on Windows:
  - https://code.visualstudio.com/
- From WSL, run `code .` at least once so VS Code installs the WSL server components.
- Install the VS Code extension: Dev Containers.
  - Install it in your VS Code (Windows side).
  - If VS Code prompts to also install it in WSL or in the container, install it there too.

## 3) Clone the Repository

From your WSL terminal:

```bash
git clone https://github.com/Long-Beach-Rocketry/Long-Beach-Rocketry-25-26.git
cd Long-Beach-Rocketry-25-26
code .
```

## 4) Open in Dev Container

In VS Code:

- Run command: Dev Containers: Reopen in Container

First startup may take several minutes while toolchains and dependencies install.

If prompted to enable or reload extensions, accept it.

## 5) Start a New Branch for Your Work

When you begin a feature or fix, create a new branch before making changes.

Use a short, descriptive branch name like `<fi><li>-<feature>` where `<fi><li>` means your first and last initial.

Example style: `ab-imu-fix` or `km-usart`.

From your current branch:

```bash
git fetch origin
git rebase origin/<default-branch>
git checkout -b <feature-branch-name>
```

If you already created a branch, switch to it with:

```bash
git checkout <feature-branch-name>
```

Keep the branch focused on one change.

## 6) Workspace Configuration

The Dev Container already applies recommended VS Code settings from .devcontainer/devcontainer.json.

Only create the files below if your workspace does not already have them.

<details>
<summary>Create .vscode/settings.json</summary>

```bash
mkdir -p .vscode
cat > .vscode/settings.json << 'EOF'
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
  "cortex-debug.gdbPath": "/usr/bin/gdb-multiarch"
}
EOF
```

</details>

<details>
<summary>Create .clangd</summary>

```bash
cat > .clangd << 'EOF'
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
    # Project include paths (relative to workspace root)
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
EOF
```

</details>

After creating these files:

1. Reopen the workspace in the Dev Container.
2. Run Command Palette -> Developer: Reload Window.

## 7) Build Firmware

Use the helper scripts at the repo root.

### Linux / WSL / Dev Container

```bash
./make.sh -t <preset>
```

Replace `<preset>` with the build preset you are using for your board or environment.

Example presets: `stm32h723`, `stm32l476`, `native`.

### Windows PowerShell

```powershell
./make.ps1 -t <preset>
```

### Useful options

- Build only one app target:

```bash
./make.sh -t <preset> -a <app-name>
```

Example app names: `blink`, `imu_test`, `usart_app`.

- Release build instead of Debug:

```bash
./make.sh -t <preset> -r
```

- Clean build:

```bash
./make.sh -t <preset> -c
```

Available presets are defined in CMakePresets.json.

## 8) Debugging with ST-Link (Windows + WSL)

1. Open Windows Terminal as Administrator.
2. Run:

```bash
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```

3. In WSL, confirm the device is visible:

```bash
lsusb
```

4. In VS Code, open Run and Debug, pick the appropriate launch configuration, then start debugging.

Note: once attached to WSL, ST-Link is available to the Dev Container.

## 9) UART Serial Monitor (Optional)

In WSL:

```bash
sudo apt install -y minicom
ls /dev/tty*
minicom -D <serial-port>
```

Use the serial device shown by `ls /dev/tty*`.

## 10) Team Workflow Tips

Use these habits to avoid merge issues and "works on my machine" problems.

- Build inside the Dev Container so you match the team and CI.

- At the start of work, and whenever main changes, fetch and rebase from your current branch.

```bash
git fetch origin
git rebase origin/<default-branch>
git checkout -b <feature-branch-name>
```

- If your branch stays open, do this again at least once a week.

- Keep each branch scoped to one feature or one fix.

- Before pushing, check what branch you are on so you do not push the wrong one.

```bash
git branch --show-current
```

- Before pushing, build the target you changed and run any relevant checks.

```bash
./make.sh -t <preset> -a <app-name>
./make.sh -t native
```

- Commit in small, meaningful chunks.

```bash
git add .
git commit -m "<short, descriptive commit message>"
git push origin <feature-branch-name>
```

- Before opening a PR, make sure `git status` is clean except for intended changes.

- If you hit conflicts, resolve them right away and rebase again.

If rebase is not preferred for a specific situation, ask before using merge instead.

## 11) Quick Troubleshooting

- Docker container does not start: check Docker Desktop and WSL integration.
- ST-Link not found: re-run the `usbipd` attach steps and verify with `lsusb`.
- Build fails due to missing tools: reopen or rebuild the Dev Container.
- VS Code looks wrong: confirm `.vscode/settings.json` and `.clangd` exist.