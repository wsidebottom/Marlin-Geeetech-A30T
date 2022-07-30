/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * Geeetech GTM32 103 V1 board pin assignments
 */

#include "env_validate.h"

#define BOARD_INFO_NAME      "GTM32 103 V1"
#define DEFAULT_MACHINE_NAME "STM32F103VET6"

#define BOARD_NO_NATIVE_USB

// Set correct timers (will lead to conflicts and instabilities otherwise)
#define TEMP_TIMER  1
#define STEP_TIMER  3

//#define DISABLE_DEBUG

//
// It is required to disable JTAG function because its pins are
// used as GPIO to drive the Y axis stepper.
// DO NOT ENABLE!
//
#define DISABLE_JTAG

//
// If you don't need the SWDIO functionality (any more), you may
// disable SWD here to regain PA13/PA14 pins for other use.
//
//#define DISABLE_JTAGSWD

// Ignore temp readings during development.
//#define BOGUS_TEMPERATURE_GRACE_PERIOD    2000

// Enable EEPROM Emulation for this board as it doesn't have EEPROM
#if EITHER(NO_EEPROM_SELECTED, FLASH_EEPROM_EMULATION)
  #define FLASH_EEPROM_EMULATION
  #define MARLIN_EEPROM_SIZE              0x1000  // 4KB
#endif

//
// Limit Switches
//
#define X_MIN_PIN                           PA15
#define X_MAX_PIN                           PC6
#define Y_MIN_PIN                           PD4
#define Y_MAX_PIN                           PD11
#define Z_MIN_PIN                           PB3
#define Z2_MIN_PIN                          PB7
#define Z2_MAX_PIN                          PD10
#define Z_PROBE_PIN                         PA1

//
// Steppers
//
#define X_STEP_PIN                          PA11
#define X_DIR_PIN                           PA12
#define X_ENABLE_PIN                        PA8

#define Y_STEP_PIN                          PD15
#define Y_DIR_PIN                           PD3
#define Y_ENABLE_PIN                        PD14

#define Z_STEP_PIN                          PD6
#define Z_DIR_PIN                           PD7
#define Z_ENABLE_PIN                        PD5

#define Z2_STEP_PIN                         PB5
#define Z2_DIR_PIN                          PB6
#define Z2_ENABLE_PIN                       PB4

#define E0_STEP_PIN                         PB9
#define E0_DIR_PIN                          PE0
#define E0_ENABLE_PIN                       PB8

#define E1_STEP_PIN                         PE4
#define E1_DIR_PIN                          PE5
#define E1_ENABLE_PIN                       PE3

#define E2_STEP_PIN                         PC14
#define E2_DIR_PIN                          PC15
#define E2_ENABLE_PIN                       PC13

//
// Heaters / Fans
//
#define HEATER_0_PIN                        PE9
#define HEATER_1_PIN                        PE11
#define HEATER_2_PIN                        PE13
#define HEATER_BED_PIN                      PE14

#define FAN_PIN                             PA6
#define FAN1_PIN                            PA7
#define FAN2_PIN                            PB1

#ifndef E0_AUTO_FAN_PIN
  #define E0_AUTO_FAN_PIN                   FAN_PIN
#endif

//
// Temperature Sensors
//
#define TEMP_0_PIN                          PC0
#define TEMP_1_PIN                          PC1
#define TEMP_2_PIN                          PC2
#define TEMP_BED_PIN                        PC3

//
// Misc. Functions
//
#define LED_PWM                             PB10  // External LED, pin 2 on LED labeled connector

//
// LCD / Controller
//
#define LCD_POWER                           PD12
#define LCD_UART_TX                         PD8
#define LCD_UART_RX                         PD9

//
// Beeper
//
#define BEEPER_PIN                          PD13

#define SDIO_SUPPORT
#define SD_DETECT_PIN                       PC7

#define SERVO0_PIN                          PA0

#define FIL_RUNOUT_PIN                      PE2
#define FIL_RUNOUT2_PIN                     PE6
#define FIL_RUNOUT3_PIN                     PE1

#define I2C_SDA                             PA13
#define I2C_SCL                             PA14
