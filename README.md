## About
This repository serves as a reference for using the Zephyr RTOS together with tools made by IAR
Systems. The repository contains Zephyr application samples for the STM32F429II-ACA board and
instructions for how to create an Embedded Workbench project based on it.

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

1. [Zephyr Setup](docs/zephyr.md)
2. [Embedded Workbench and IAR Tools Setup](docs/embedded-workbench.md)
3. [Required Environment Variables](docs/environment-variables.md)
4. [Initial build and creating project](docs/creating-project.md)
5. [Compiling and Debugging with Embedded Workbench](docs/compiling-debugging.md)
6. [Visual Studio Code Integration](docs/vscode-integration.md)

### Miscellaneous Operations

#### Generating Zip Files For Boards
Use this command template when wanting to generate a zip file for a specific board folder:

```bash
git archive -o <board_name>.zip HEAD:boards/<board_name>
```
