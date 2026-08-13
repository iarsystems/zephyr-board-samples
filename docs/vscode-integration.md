
## Visual Studio Code integration
IAR maintains two VS Code extensions which embed compiling and debugging functionality of Embedded Workbench inside of VS Code.

### Installation
The easiest way to download the extensions is from the VS Code Marketplace:

1. From within VS Code, press **Ctrl+Shift+x** to open the Extensions panel.
2. Paste `@publisher:IAR` into the search bar. This will bring up the extensions maintained by IAR.
3. Install `IAR Tools Extension Pack` to install all relevant extensions.

### Opening an Embedded Workbench Project in VS Code
1. Open the folder containing the Embedded Workbench project you want to test by pressing
   **Ctrl+k Ctrl+o**, or navigating to **File -> Open Folder...**
2. After you installed the extensions in the previous step, a new view "IAR Build" should have
   popped up in the Activity Bar to the left which you can click on.
3. From the "IAR Build" view, under the "Extension Configuration" section specify the path to the
   IAR Embedded Workbench installation folder as shown in the image below:

    ![IAR VS Code Extension Configuration](images/vscode-iar-config.jpg)

4. Both the workspace and the project files should automatically be detected from the folder you opened in VS Code, as shown in the above image.

Now you are ready to build and debug an Embedded Workbench project from inside VS Code.

### Building with the IAR Build Extension
1. To start building an Embedded Workbench project, first navigate to **Terminal -> Run Task**.
2. In the search bar that pops up, search for "iar" and then select "iar".
3. Select "iar: Build Project".

This will simply build the project inside of VS Code according to the settings specified in the Embedded Workbench project.
When you build the project, a terminal opens up showing the build output messages.

### Debugging with the IAR C-SPY Debug Extension
1. To prepare for debugging in VS Code, first copy `launch.json` from the build directory to the
   `.vscode` directory.
2. Remove the `"plugins"` property in the new `launch.json`.

To start the debug session, simply press **F5** or navigate to **Run -> Start Debugging**.

Once the debug session has been loaded up, the debug toolbar shown in the image below will appear which can be used to control the debugging session:

![VS Code Debug Toolbar](images/vscode-debug-toolbar.jpg)

---

[Previous: Compiling and Debugging with Embedded Workbench](./compiling-debugging.md)

[Back to README](../README.md)
