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
 * lcd/extui/geeetech_a30t/geeetech_a30t.h
 */

#include "../../../HAL/shared/Marduino.h"

/**
 * @brief Receive a command from the display. 
 * 
 * @return String the received command. Line number and checksum are already removed.
 */
String receiveCommand();

/**
 * @brief Check whether a command needs processing or can just be queued as GCODE.
 * 
 * @param command The command to check
 * @return whether the command can just be enqueued without further processing
 */
bool canForwardToQueue(String command);

/**
 * @brief queue a gcode command for the printer
 * 
 * @param gcode the command to queue. Should be valid GCODE.
 */
void queueGcode(String gcode);