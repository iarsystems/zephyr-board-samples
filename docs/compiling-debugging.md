## Compiling and Debugging with EWARM
Once you have everything set up from the previous steps, you are ready to start compiling and debugging demo projects using Embedded Workbench.

### Creating Initial Builds with West
Before building and debugging one of the sample projects inside Embedded Workbench, you first have to make an initial build using Zephyr's ``west`` command line tool.
The reason for this is because the initial build using ``west`` produces necessary build files and configurations for your specific application and target system that gets cached in the build folder for later builds with Embedded Workbench.

``west`` should be installed and ready to use after having followed the [Zephyr Setup Guide](./zephyr.md).
Follow one of the below sections depending on the board you are building for, replacing the paths in the command with the correct paths for your system:

#### nRF52840 DK
```bash
west build -b nrf52840dk/nrf52840 -d <path-to-nrf52840dk-folder>/workbench/build <path-to-nrf52840dk-folder>/led_carousel -p -- -DCONFIG_USE_SEGGER_RTT=n
```

#### micro:bit
```bash
west build -b bbc_microbit -d <path-to-microbit-folder>/workbench/build <path-to-microbit-folder>/snake -p
```

#### FRDM-MCXN947
```bash
west build -b frdm_mcxn947/mcxn947/cpu0 -d <path-to-frdm_mcxn947-folder>/workbench/build <path-to-frdm_mcxn947-folder>/blinky -p
```

Building with ``west`` is only necessary for the first build.
Afterwards, for each modification to the program it can be compiled using EWARM.

### Building a Project using EWARM
Once you have done an initial build using ``west`` of the sample project you are testing, it is ready to be built with EWARM:

1. Open up the EWARM IDE. From your EWARM installation folder, the executable for the IDE is located at *``common\bin\IarIdePm.exe``*.
2. Navigate to **File -> Open Workspace...**
3. In your file browser, navigate to the *``workbench``* folder of the sample you want to build for and select the workspace file (.eww file extension, "IAR IDE Workspace" file type).
4. Press **F7** or navigate to **Project -> Make** to build the project.

This will build the project with the output folder being the same build folder used in the ``west`` command.
Each time a change is made to the application being built, it is detected and will be compiled when building with **Project -> Make**.
If you want to rebuild the project again regardless of if there are no changes, navigate to **Project -> Rebuild All**.

### Debugging a Project using EWARM
1. Make sure the target device is connected to your computer.
    > [!NOTE]
    > It is a good idea to not have multiple devices connected and only the target device.
    >
    > This may simplify device selection for the debugger,
    > and might be especially of note if you have two or more devices that use the same debugger driver.
    >
2. Start a debugging session by pressing **Ctrl+D** or navigating to **Project -> Download and Debug**.
3. If the debugging session started successfully, you will get this menu in the toolbar to control the debugging session:

    ![Debugging tool](images/ewarm-debug-toolbar.png)

#### Zephyr Debugger Plugin
There are many features of the debugger which are beyond the scope of this documentation.
One specific thing that has been configured for each sample project is the Embedded Workbench Zephyr debugger plugin, which provides integrated Zephyr-specific debugging tools.

While you are debugging, you can navigate to **Zephyr** in the menu bar.
Here you can choose from a few things, like for instance **Thread List**, which opens a window that gives information about threads in the application being debugged.

---

[Next: Visual Studio Code Integration](./vscode-integration.md)

[Previous: Required Environment Variables](./environment-variables.md)

[Back to README](../README.md)
