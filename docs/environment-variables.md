## Required Environment Variables
There are a few environment variables that need to be set before the next steps:

```powershell
# PowerShell
$Env:ZEPHYR_TOOLCHAIN_VARIANT="iar"
$Env:IAR_TOOLCHAIN_PATH=<your-toolchain-path-here>\arm
$Env:IAR_LMS_BEARER_TOKEN="<your-license-bearer-token-here>"
```

```bash
# Cmd
set ZEPHYR_TOOLCHAIN_VARIANT="iar"
set IAR_TOOLCHAIN_PATH=<your-toolchain-path-here>\arm
set IAR_LMS_BEARER_TOKEN="<your-license-bearer-token-here>"
```

``ZEPHYR_TOOLCHAIN_VARIANT`` is used by Zephyr to set the compiler toolchain variant to use.
In this case, it is set to ``"iar"`` in order for Zephyr to use IAR's compiler toolchain to compile applications.

``IAR_TOOLCHAIN_PATH`` is used to set the path to the IAR compiler toolchain, which is needed for the next steps when compiling an initial build using Zephyr tools.
There are two options for this, and depending on what you installed in the [previous guide](./embedded-workbench.md) you can choose at least option 1: 

1. It can be set to EWARM's built-in ARM compiler toolchain at *``<path-to-your-EWARM-installation>\arm``*.
2. If installed in the previous guide, it can be set to the standalone IAR ARM Build Tools at *``<path-to-your-iar-arm-build-tools-installation>\arm``*.

``IAR_LMS_BEARER_TOKEN`` is used to set a valid IAR license bearer token, which is required to authenticate your usage of IAR tools.

---

[Next: Compiling and Debugging with Embedded Workbench](./compiling-debugging.md)

[Previous: Embedded Workbench and IAR Tools Setup](./embedded-workbench.md)

[Back to README](../README.md)
