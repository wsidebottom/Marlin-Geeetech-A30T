# Marlin 3D Printer Firmware

![GitHub](https://img.shields.io/github/license/marlinfirmware/marlin.svg)
![GitHub contributors](https://img.shields.io/github/contributors/marlinfirmware/marlin.svg)
![GitHub Release Date](https://img.shields.io/github/release-date/marlinfirmware/marlin.svg)
[![Build Status](https://github.com/MarlinFirmware/Marlin/workflows/CI/badge.svg?branch=bugfix-2.0.x)](https://github.com/MarlinFirmware/Marlin/actions)

<img align="right" width=175 src="buildroot/share/pixmaps/logo/marlin-250.png" />

Additional documentation can be found at the [Marlin Home Page](https://marlinfw.org/).
Please test this firmware and let us know if it misbehaves in any way. Volunteers are standing by!

## 3D Print Mill Kinematics

Historically belt printers have interpreted the X Y Z parameters as referring to the positions of the XY gantry in its own plane, and the Z belt position in its own plane. As a result, extra processing has been required to produce G-code suitable for the scheme. In the past, this was also the case for SCARA and Delta printers. Slicers have started to fall in line with the de-facto standard, so the time is nigh to correct the kinematics.

There are two components to the problem:

  - __The slicer__ needs to produce G-code that aligns properly to the belt / gantry, but with supports as if the belt was the bottom of the model. However, the slicer has to rotate the solid model and generated support geometry together by the machine angle (_e.g._, 45°) before slicing 'as usual' in the Z plane.

  - __The firmware__ needs to know what Y coordinate corresponds to Z0 before it starts the print. The firmware unfortunately can't easily guess. The "height of the first layer" on this machine is semi-meaningless, because it's guaranteed to keep drawing lines on the belt at some regular height throughout the print, and babystepping might be a very regular need. We *could guess* by using the volume what the height is supposed to be off of the belt, and use that to tune the relationship between Y0 and Z0.

  How is Y related to Z in the G-code and why is this important? All prints, even on a belt, will start with Z0 and then will continue with Z at the first layer height. Since a model may be sliced anywhere in the Y space, we need to know which Y position it "starts at" and seed the kinematics appropriately.

  Anyway, the thing is.... Once the kinematics or workspace offset are seeded, the Y and Z axes need to be considered together before applying Core kinematics and conversion into steps. It gets a tiny bit tricky, but shouldn't be too impossible to follow. The formula is applied in the same location as the CoreXY stuff:

    1. Convert Y and Z inputs into YB and C, where YB is the Y position of the XY gantry in its own plane.
    2. Hand YB over to CoreXY kinematics where Y would normally go.
    3. For babystepping of "the squish side" just allowing direct Y adjustment is enough. No other babystepping is needed.

  The formula to convert for Y and Z into YB and C is... Notes!
    - Cartesion `Y0 Z0` converts to `YB0 C0`. The "YB0 offset" is the adjustment made to the YB0 position, which will keep the squished side against the belt, even as the G-code Y position is going ever-downward, or ever-upward. Figure out which way is more correct. I assume Y goes downward. (Maybe both angular variants can be supported with extra `G333` parameters. Why 333? Because the Belt of Orion...)
    - For simplicity the YB0 offset simply corresponds to the current Z position, so 'Z' can be taken as ruling over YB0. This could be expressed as a steps value which is adjusted whenever Z is changed, or it can simply be based on current C steps with a clever formula.
    - Note that when YB is at the machine native home point (0), it's as close to the belt as it can get (though, this could be usefully adjusted with a Y home offset) so it's at the minimum the Z layer height.
    - The YB position alone determines the height of the nozzle over the belt, hence the actual "squish level" on the belt side of the model. For these purposes it's better to think of this as "the belt side" rather than the bottom.
    - So, how much should the YB coordinate be messed with in relation to Z?
    - YB = Y - Z * sin(angle) ; C = Z * (1 / sin(angle))
    - The ratio between Z motion and Belt motion is (1 / sin(angle)) => Hypot (YB) / Adjacent (Z)
    - For every +1mm that C moves, YB needs to be offset by -1.414mm.
    - For every +1.414mm that YB moves, C needs to be offset by -1mm.
    - Position `Y0 Z10` is `YB... C14.14`

  I suggest requiring a G-code that both enables *correct* Belt Cartesian Kinematics (BCK), but also sets the Y0/Z0 relationship at the same time. Assuming BCK is the default, the slicer could easily set this value with an existing command like `G92 Y123.2`. If BCK needs to be enabled, maybe a Y offset could be included.

## Marlin 2.0

Marlin 2.0 takes this popular RepRap firmware to the next level by adding support for much faster 32-bit and ARM-based boards while improving support for 8-bit AVR boards. Read about Marlin's decision to use a "Hardware Abstraction Layer" below.

Download earlier versions of Marlin on the [Releases page](https://github.com/MarlinFirmware/Marlin/releases).

## Building Marlin 2.0

To build Marlin 2.0 you'll need [Arduino IDE 1.8.8 or newer](https://www.arduino.cc/en/main/software) or [PlatformIO](http://docs.platformio.org/en/latest/ide.html#platformio-ide). Detailed build and install instructions are posted at:

  - [Installing Marlin (Arduino)](http://marlinfw.org/docs/basics/install_arduino.html)
  - [Installing Marlin (VSCode)](http://marlinfw.org/docs/basics/install_platformio_vscode.html).

### Supported Platforms

  Platform|MCU|Example Boards
  --------|---|-------
  [Arduino AVR](https://www.arduino.cc/)|ATmega|RAMPS, Melzi, RAMBo
  [Teensy++ 2.0](http://www.microchip.com/wwwproducts/en/AT90USB1286)|AT90USB1286|Printrboard
  [Arduino Due](https://www.arduino.cc/en/Guide/ArduinoDue)|SAM3X8E|RAMPS-FD, RADDS, RAMPS4DUE
  [LPC1768](http://www.nxp.com/products/microcontrollers-and-processors/arm-based-processors-and-mcus/lpc-cortex-m-mcus/lpc1700-cortex-m3/512kb-flash-64kb-sram-ethernet-usb-lqfp100-package:LPC1768FBD100)|ARM® Cortex-M3|MKS SBASE, Re-ARM, Selena Compact
  [LPC1769](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc1700-cortex-m3/512kb-flash-64kb-sram-ethernet-usb-lqfp100-package:LPC1769FBD100)|ARM® Cortex-M3|Smoothieboard, Azteeg X5 mini, TH3D EZBoard
  [STM32F103](https://www.st.com/en/microcontrollers-microprocessors/stm32f103.html)|ARM® Cortex-M3|Malyan M200, GTM32 Pro, MKS Robin, BTT SKR Mini
  [STM32F401](https://www.st.com/en/microcontrollers-microprocessors/stm32f401.html)|ARM® Cortex-M4|ARMED, Rumba32, SKR Pro, Lerdge, FYSETC S6
  [STM32F7x6](https://www.st.com/en/microcontrollers-microprocessors/stm32f7x6.html)|ARM® Cortex-M7|The Borg, RemRam V1
  [SAMD51P20A](https://www.adafruit.com/product/4064)|ARM® Cortex-M4|Adafruit Grand Central M4
  [Teensy 3.5](https://www.pjrc.com/store/teensy35.html)|ARM® Cortex-M4|
  [Teensy 3.6](https://www.pjrc.com/store/teensy36.html)|ARM® Cortex-M4|
  [Teensy 4.0](https://www.pjrc.com/store/teensy40.html)|ARM® Cortex-M7|
  [Teensy 4.1](https://www.pjrc.com/store/teensy41.html)|ARM® Cortex-M7|

## Submitting Changes

- Submit **Bug Fixes** as Pull Requests to the ([bugfix-2.0.x](https://github.com/MarlinFirmware/Marlin/tree/bugfix-2.0.x)) branch.
- Follow the [Coding Standards](http://marlinfw.org/docs/development/coding_standards.html) to gain points with the maintainers.
- Please submit your questions and concerns to the [Issue Queue](https://github.com/MarlinFirmware/Marlin/issues).

## Marlin Support

For best results getting help with configuration and troubleshooting, please use the following resources:

- [Marlin Documentation](http://marlinfw.org) - Official Marlin documentation
- [Marlin Discord](https://discord.gg/n5NJ59y) - Discuss issues with Marlin users and developers
- Facebook Group ["Marlin Firmware"](https://www.facebook.com/groups/1049718498464482/)
- RepRap.org [Marlin Forum](http://forums.reprap.org/list.php?415)
- [Tom's 3D Forums](https://forum.toms3d.org/)
- Facebook Group ["Marlin Firmware for 3D Printers"](https://www.facebook.com/groups/3Dtechtalk/)
- [Marlin Configuration](https://www.youtube.com/results?search_query=marlin+configuration) on YouTube

## Credits

The current Marlin dev team consists of:

 - Scott Lahteine [[@thinkyhead](https://github.com/thinkyhead)] - USA &nbsp; [Donate](http://www.thinkyhead.com/donate-to-marlin)
 - Roxanne Neufeld [[@Roxy-3D](https://github.com/Roxy-3D)] - USA
 - Chris Pepper [[@p3p](https://github.com/p3p)] - UK
 - Bob Kuhn [[@Bob-the-Kuhn](https://github.com/Bob-the-Kuhn)] - USA
 - Erik van der Zalm [[@ErikZalm](https://github.com/ErikZalm)] - Netherlands &nbsp; [![Flattr Erik](https://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=ErikZalm&url=https://github.com/MarlinFirmware/Marlin&title=Marlin&language=&tags=github&category=software)

## License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
