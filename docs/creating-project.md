## Initial build and creating project
Before building and debugging one of the sample projects inside Embedded Workbench, you first have
to make an initial build using Zephyr's `west` command line tool. The reason for this is because
the initial build using `west` produces necessary build files and configurations for your
specific application and target system that gets cached in the build folder for later builds with
Embedded Workbench.

### Initial build
`west` should be installed and ready to use after having followed the [Zephyr Setup Guide](./zephyr.md).
Follow the instructions for the sample project you want to build, replacing the paths in the
command with the correct paths for your system:

#### Test leds
```bash
west build -b stm32f429ii_aca -d <path-to-desired-build-directory> <path-to-this-repository>/boards/stm32f429ii_aca/test_leds -p
```

#### Toggle car light
```bash
west build -b stm32f429ii_aca -d <path-to-desired-build-directory> <path-to-this-repository>/boards/stm32f429ii_aca/toggle_car_light -p
```

#### Traffic situation
```bash
west build -b stm32f429ii_aca -d <path-to-desired-build-directory> <path-to-this-repository>/boards/stm32f429ii_aca/traffic_situation -p
```

Building with `west` is only necessary for the first build.
Afterwards, for each modification to the program it can be compiled using EWARM.

### Generate launch.json
EWARM uses a file named `launch.json` to set debugger settings. This file can be generated with the
`west debug` command. This way EWARM will have the correct device selected and the Zephyr Debugger
plugin will be enabled as soon as the EWARM project is created. 

At this step you have to specify the probe if you are not using the default. This is done with the
`--driver` option. You can also specify a symbol to break at (e.g. `main`) with the
`--stop-on-symbol` option.

```bash
west debug -r iar -g -d <path-to-build-directory-from-build-command>
```

> [!NOTE]
> If you omit the `-g` flag, Embedded Workbench will launch with a temporary project to debug
> immediately, which is useful if you only want to debug and not develop in EWARM

### Create EWARM project
Now that you have an initial build of the project, you can create an EWARM project for it. 

1. Open up the EWARM IDE. From your EWARM installation folder, the executable for the IDE is
   located at *`common\bin\iaride.exe`*.
2. Navigate to **Project -> Add or Create -> Create New Project...**
3. Select `Attach to pre-configured CMake directory`.
4. Browse to the build directory created by west.
5. Select where to save the EWARM project files.

Now you have an EWARM project where you can build, develop and debug your zephyr application. 

---

[Next: Compiling and Debugging with Embedded Workbench](./compiling-debugging.md)

[Previous: Required Environment Variables](./environment-variables.md)

[Back to README](../README.md)
