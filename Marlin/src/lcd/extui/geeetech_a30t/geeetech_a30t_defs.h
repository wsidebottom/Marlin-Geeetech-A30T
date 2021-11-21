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
 * lcd/extui/geeetech_a30t/geeetech_a30t_defs.h
 */

// settings
#define DISPLAY_DEBUG // comment out for production
#define MAX_RECEIVE_COMMANDS 10
#define STATUS_CYCLE_IN_MS 1000

// status values
#define PRINT_STATUS_IDLE 0
#define PRINT_STATUS_PRINTING 1
#define PRINT_STATUS_PAUSED 2
#define PRINT_STATUS_FINISHED 4

#define FILAMENT_SENSOR_DISABLED 255
#define FILAMENT_SENSOR_OUT 0
#define FILAMENT_SENSOR_IN 1

// status macros
#define FEEDRATE (uint8_t) round(ExtUI::getFeedrate_mm_s())
#define BED_ACTIVE thermalManager.isHeatingBed() ? 1 : 0
#define E0_ACTIVE thermalManager.isHeatingHotend(0) ? 1 : 0
#define SD_ACTIVE isMediaInserted() ? 0 : 1
#define F0_SPEED (uint8_t) round(getActualFan_percent(FAN0))
#define PRINT_SPEED (uint8_t) round(getFeedrate_percent())
#define FILAMENT_SENSOR_STATUS getFilamentRunoutEnabled() ? (getFilamentRunoutState() ? FILAMENT_SENSOR_IN : FILAMENT_SENSOR_OUT) : FILAMENT_SENSOR_DISABLED
#define CURRENT_FILENAME card.filename
#define MOTOR_TENSION_STATUS stepper.axis_is_enabled(X_AXIS) || stepper.axis_is_enabled(Y_AXIS) || stepper.axis_is_enabled(Z_AXIS) || stepper.axis_is_enabled(E_AXIS)
