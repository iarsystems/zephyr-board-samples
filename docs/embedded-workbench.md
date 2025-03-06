## Embedded Workbench and IAR Tools Setup
Embedded Workbench is an IDE produced by IAR Systems that is specialized for work with embedded systems.
It includeds tools such as a code editor, a compiler, a linker, and a debugger, and supports many different architectures and devices.
In each demo project provided through this repository, a subfolder named *``workbench``* is included which contains the files necessary to run each demo project inside of Embedded Workbench.

### Installation

#### Embedded Workbench for ARM
Embedded Workbench for ARM, or EWARM for short, is fitted to work with systems that use the ARM processor architecture.
Install version 9.70.X or higher of EWARM to get a version that supports work with Zephyr.

#### Optional: IAR Standalone Compiler Toolchain
Embedded Workbench is a fully-fledged IDE, containing all the necessary tools to program, build, and debug a project.
However, there is a possibility to install a standalone version of the IAR build tools.
Simply head over to [this github releases page](https://github.com/iarsystems/zephyr-iar/releases) to download and install the latest version of the IAR ARM build tools somewhere on your system.
This can be used later to compile an initial build for a sample project.

---

[Next: Required Environment Variables](./environment-variables.md)

[Previous: Zephyr Setup](./zephyr.md)

[Back to README](../README.md)
