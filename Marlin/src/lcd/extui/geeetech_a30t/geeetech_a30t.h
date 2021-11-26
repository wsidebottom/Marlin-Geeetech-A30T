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
        // public
        static void startup();
        static void process();
        static void waitForCommand(const bool ignore);

    private:
        static bool shouldWaitForCommand;
        static char levelCenter_xPosString[7];
        static char levelCenter_yPosString[7];

        //status variables
        static millis_t nextTempDataUpdate;
        static char bedCurrentTemp[6], bedTargetTemp[6], e0CurrentTemp[6], e0TargetTemp[6];
        //status methods
        static void updateTemperatureDataIfNeeded(const millis_t &currentTimeMs);

        // send variables
        static millis_t nextStatusSend;
        static bool disableStatusSend;
        static char output[200]; // should be enough for all message
        // send methods
        static void setNextSendMs(const millis_t &currentTimeMs);
        static void sendStatusIfNeeded(const millis_t &currentTimeMs);
        static void send_L1_AxisInfo();
        static void send_L2_TempInfo();
        static void send_L3_PrintInfo();
        static void send_L9_FirmwareInfo();
        static void send_L10_ZOffset();
        static void send_L11_ProbeZOffset();
        static void sendToDisplay(PGM_P message, const bool addChecksum = true);
        static uint32_t getMixerRatio();
        static uint8_t getPrintStatus();

        // receive methods
        static UiCommand receiveCommand();
        static String receiveCommandString();
        static UiCommand parseCommandString(const String &commandString);
        static CommandType parseCommandType(const String &commandString);
        static void parseCommandParameters(UiCommand &command, const String &commandString);

        // handle
        static void handleGcode(const String &command);
        static void handleUnkownCommand(const UiCommand &command);
        static void handleProprietaryCommand(const UiCommand &command);

        // M2107
        static char levelRR_xPosString[7];
        static char levelRR_yPosString[7];
        static char levelRL_xPosString[7];
        static char levelRL_yPosString[7];
        static char levelFL_xPosString[7];
        static char levelFL_yPosString[7];
        static char levelFR_xPosString[7];
        static char levelFR_yPosString[7];
        static void handle_M2107_ManualLeveling(const UiCommand &command);
        static void handle_M2107_S0_Start();
        static void handle_M2107_S8_OkSave();
        static void moveToXYWithZHop(const char *xPos, const char *yPos);
        static void moveUpDownSmallBigStep(const bool &up, const bool &bigStep);

        // M2120
        static bool simulatedAutoLevelSwitchOn;
        static void handle_M2120_AutoLeveling(const UiCommand &command);
        static void handle_M2120_P1_ProbeControl(const char &sParameter);
        static void handle_M2120_P2_StoreZOffset(const String &sParameter);
        static void handle_M2120_P3_MoveUp(const char &sParameter);
        static void handle_M2120_P4_MoveDown(const char &sParameter);
        static void handle_M2120_P7_ProbeCenter();
        static void handle_M2120_P6_CenterNozzle();
        static String mapSParameterToHeightString(const char &sParameter);

        // M2134
        static void handle_M2134(const UiCommand &command);
    };

    extern TouchDisplay Display;
}