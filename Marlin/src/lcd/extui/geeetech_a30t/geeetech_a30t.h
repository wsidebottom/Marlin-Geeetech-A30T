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

#include "geeetech_a30t_defs.h"

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
        FOREACH_COMMAND(GENERATE_ENUM)
    };
    static const char *COMMAND_STRINGS[] = {FOREACH_COMMAND(GENERATE_STRING)};
    static const uint8_t COMMANDS_COUNT = FOREACH_COMMAND(GENERATE_COUNT);

    enum Parameter
    {
        FOREACH_PARAMETER(GENERATE_ENUM)
    };
    static const char *PARAMETER_STRINGS[] = {FOREACH_PARAMETER(GENERATE_STRING)};
    static const uint8_t PARAMETERS_COUNT = FOREACH_PARAMETER(GENERATE_COUNT);

    struct UiCommand
    {
        CommandType type;
        String command;
        String parameters[PARAMETERS_COUNT];
    };

    class TouchDisplay
    {
    public:
        static void startup();
        static void receiveAndProcess();
        static void ignoreCommands(const bool ignore);

    private:
        static void sendStatusIfNeeded(const millis_t *currentTimeMs);
        static void sendL1AxisInfo();
        static void sendL2TempInfo();
        static void sendL3PrintInfo();
        static void sendToDisplay(PGM_P message, const bool addChecksum = true);

        static uint32_t getMixerRatio();
        static uint8_t getPrintStatus();

        static void receiveCommands();
        static String receiveCommandString();
        static UiCommand parseCommandString(const String commandString);
        static CommandType parseCommandType(const String commandString);
        static void parseCommandParameters(UiCommand *command, String commandString);

        static void handleGcode(const String *command);
        static void handleUnkownCommand(const UiCommand *command);
        static void handleProprietaryCommand(const UiCommand *command);

        static void handleM2120(const UiCommand *command);
    };

    extern TouchDisplay Display;
}