## About
This repository serves as a reference for using the Zephyr RTOS together with tools made by IAR Systems.
The repository contains Zephyr application samples for various boards combined with IAR tools integration, and related tutorials linked to in this README.

### Board Samples Folder Structure
Each board demo is located as a subfolder in the *``boards``* parent directory.
Each board folder contains two subfolders, one for the Zephyr sample application, and one named *``workbench``* to be used with IAR tools.

## Guides
> [!IMPORTANT]
> The guides assume that you are running Windows as your operating system.
>
> It is also assumed that you have access to, and have an understanding of the IAR organization-exclusive tools.
>

### Getting Started
> [!IMPORTANT]
> It is highly recommended to follow these guides as they are ordered below.
>

1. [Device Specifics](docs/device-specifics.md)
2. [Zephyr Setup](docs/zephyr.md)
3. [Embedded Workbench and IAR Tools Setup](docs/embedded-workbench.md)
4. [Required Environment Variables](docs/environment-variables.md)
5. [Compiling and Debugging with Embedded Workbench](docs/compiling-debugging.md)
6. [Visual Studio Code Integration](docs/vscode-integration.md)

### Miscellaneous Operations

#### Generating Zip Files For Boards
Use this command template when wanting to generate a zip file for a specific board folder:

```bash
git archive -o <board_name>.zip HEAD:boards/<board_name>
```
