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

/**
 * lcd/extui/geeetech_a30t/geeetech_a30t_base.cpp
 *
 * This is the implementation of the communication interface with the
 * proprietary touch TFT that comes with the Geeetech A30T printer.
 * 
 * Basic information about the connection to the display:
 * The display is powered by 5V over a connector on the board. The 5V is, however,
 * not enabled right away but has to be switched on through a transistor connected
 * to pin PD12. Pulling this up makes the display start up.
 * 
 * The communication is handled through an UART interface with RX and TX pins being
 * PD9 and PD8 respectively.
 * 
 * The communication happens through a mix of GCODE and additional GCODE-like commands
 * that are not standard (either L-codes or M-codes with very high numbers). While
 * playing around with the LCD I collected some information in a github repo
 * (link below). I'll implement the commands that are most helpful and throw in a
 * handful of other things as well as I deem nice to have.
 *
 * https://github.com/TheThomasD/GeeetechA30T
 * 
 * written in 2021 by TheThomasD
 */

#include "../../../inc/MarlinConfigPre.h"

#if ENABLED(GEEETECH_A30T_TFT)

#include "geeetech_a30t.h"
#include "../ui_api.h"
#include "../../marlinui.h"

#include "../../../sd/cardreader.h"
#include "../../../module/temperature.h"
#include "../../../module/stepper.h"
#include "../../../module/motion.h"
#include "../../../libs/duration_t.h"
#include "../../../module/printcounter.h"
#include "../../../gcode/queue.h"

String receiveCommand()
{
    String receivedLine = LCD_SERIAL.readStringUntil('\n');
    int lastStarChar = receivedLine.lastIndexOf('*');

    // just remove checksum without checking :-0
    String result = receivedLine;
    if (lastStarChar > 0)
        result = receivedLine.substring(0, lastStarChar);

    // have to do "while" here because at least one command uses two times "N-0" :(
    while (result.startsWith("N-0"))
        result = result.substring(4);

    return result;
}

bool canForwardToQueue(String command)
{
    bool result = false;

    if ('G' == command.charAt(0) && 'e' != command.charAt(1)) // exclude Geeetech
        result = true; // G-based codes are OK
    else if ('M' == command.charAt(0) && command.indexOf(' ') <= 4)
        result = true; // M-based codes are OK if number is max 3 digits

    return result;
}

void queueGcode(String gcode)
{
    queue.enqueue_one(gcode.c_str());
}

#endif // GEEETECH_A30T_TFT