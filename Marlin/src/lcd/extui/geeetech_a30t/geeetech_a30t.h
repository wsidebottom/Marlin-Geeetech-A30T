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

#include "../../../sd/cardreader.h"
#include "../../../module/temperature.h"
#include "../../../module/stepper.h"
#include "../../../module/motion.h"
#include "../../../libs/duration_t.h"
#include "../../../module/printcounter.h"
#include "../../../gcode/queue.h"

namespace Geeetech
{

    enum CommandType
    {
        Unknown,
        GCode,
        Proprietary
    };

    struct UiCommand
    {
        CommandType type;
        String command;
    };

    class TouchDisplay
    {
    public:
        static void startup();
        static void onIdle();
        static void ignoreCommands(bool ignore);
    private:
        static void sendStatus();
        static void receiveCommands();
        static bool isProprietaryCommand(String command);
        static bool canForwardToQueue(String command);
        static void queueGcode(String command);
        static String receiveCommand();
        static void sendToDisplay(PGM_P message, bool addChecksum = true);
        static void sendL1AxisInfo();
        static void sendL2TempInfo();
        static void sendL3PrintInfo();
        static uint32_t getMixerRatio();
        static uint8_t getPrintStatus();
    };

    extern TouchDisplay Display;
}