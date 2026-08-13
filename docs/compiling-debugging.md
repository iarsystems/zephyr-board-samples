## Compiling and Debugging with EWARM
Once you have everything set up from the previous steps, you are ready to start compiling and
debugging your project using Embedded Workbench.

### Building a Project using EWARM
Once you have done an initial build using `west` of the sample project you are testing, it is ready
to be built with EWARM. To build your project in EWARM press **F7** or navigate to
**Project -> Make** to build the project.

This will build the project with the output folder being the same build folder used in the `west`
command. Each time a change is made to the application being built, it is detected and will be
compiled when building with **Project -> Make**. If you want to rebuild the project again
regardless of if there are no changes, navigate to **Project -> Rebuild All**.

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

[Previous: Initial build and creating project](./creating-project.md)

[Back to README](../README.md)
