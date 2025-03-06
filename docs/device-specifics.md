## Device Specifics

### J-Link
J-Link is used as the default debugger driver for two boards, the NXP FRDM_MCXN947, and the Nordic Semiconductor nRF52840 DK.
Download and install the latest version of the J-Link software [here](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack).

### NXP FRDM_MCXN947
The sample project for the FRDM_MCXN947 has been configured to use J-Link as the debugger driver.
However, the factory default for this device is to use its onboard MCU-Link debugger which uses the CMSIS-DAP protocol.
If your board has not been configured to use J-Link, you can either enable it, or later change the Embedded Workbench project settings for this board so that CMSIS-DAP is used as the debugger driver.
More information about configuring a debug probe for this device can be found [here](https://docs.zephyrproject.org/latest/boards/nxp/frdm_mcxn947/doc/index.html#configuring-a-debug-probe).

### BBC micro:bit
The sample project for the micro:bit has been tested on a micro:bit V1.5.
It has not yet been tested on a micro:bit V2, although it should be easy to port to.

---

[Next: Zephyr Setup](./zephyr.md)

[Back to README](../README.md)
