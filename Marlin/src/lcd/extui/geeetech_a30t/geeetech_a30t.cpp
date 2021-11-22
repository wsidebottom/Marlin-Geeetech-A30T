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

using namespace ExtUI;

namespace Geeetech
{
    UiCommand receivedCommands[MAX_RECEIVE_COMMANDS];
    uint8_t receivedCommandsCount = 0;
    millis_t nextStatusSend = 0;
    bool ignoreIncomingCommands = false;
    bool simulatedAutoLevelSwitchOn = true;

    void TouchDisplay::startup()
    {
        // use pin PD12 (60) as output and pull it up to supply 5V power
        SET_OUTPUT(60);
        WRITE(60, 1);

        // setup the communication with the display
        LCD_SERIAL.setRx(LCD_UART_RX); // use PD9 for RX
        LCD_SERIAL.setTx(LCD_UART_TX); // use PD8 for TX
        LCD_SERIAL.begin(115200);      // Geeetech uses 115200 speed(

#ifdef GEEETECH_DISPLAY_DEBUG
        SERIAL_ECHOLNPGM("Geeetech A30T TFT initialized");
#endif
    }

    void TouchDisplay::receiveAndProcess()
    {
        const millis_t currentTimeMs = millis();
        sendStatusIfNeeded(&currentTimeMs);

        receiveCommands();

        for (uint8_t i = 0; !ignoreIncomingCommands && i < receivedCommandsCount; i++)
        {
            UiCommand command = receivedCommands[i];
#ifdef GEEETECH_DISPLAY_DEBUG
            SERIAL_ECHOLNPGM("CommandType: ", COMMAND_STRINGS[command.type]);
            SERIAL_ECHOLNPGM("String: ", command.command.c_str());
            for (uint8_t j = 0; j < PARAMETERS_COUNT; j++)
                SERIAL_ECHOLNPGM(PARAMETER_STRINGS[j], command.parameters[j].c_str());
#endif

            if (ignoreIncomingCommands)
                break;

            if (GCode == command.type)
                handleGcode(&(command.command));
            else if (Unknown == command.type)
                handleUnkownCommand(&command);
            else
                handleProprietaryCommand(&command);

            // schedule an immediate answer, if needed
            if (false FOREACH_ANSWER(command.type, GENERATE_BOOL_COMPARE))
                nextStatusSend = currentTimeMs;
        }
    }

    void TouchDisplay::receiveCommands()
    {
        receivedCommandsCount = 0;
        while (receivedCommandsCount < MAX_RECEIVE_COMMANDS && LCD_SERIAL.available())
            receivedCommands[receivedCommandsCount++] = parseCommandString(receiveCommandString());
    }

    UiCommand TouchDisplay::parseCommandString(const String commandString)
    {
        UiCommand result = {}; // init
        result.type = parseCommandType(commandString);

        if (GCode == result.type)
            result.command = commandString;
        else if (Unknown != result.type)
            parseCommandParameters(&result, commandString);

        return result;
    }

    void TouchDisplay::parseCommandParameters(UiCommand *command, const String commandString)
    {
        String currentCommand = COMMAND_STRINGS[command->type];
        if (commandString.length() + 1 <= currentCommand.length())
            return; // no parameters detected

        String parameterString = commandString.substring(currentCommand.length() + 1);
        for (uint8_t i = 0; i < PARAMETERS_COUNT; i++)
        {
            parameterString.trim();
            if (parameterString.length() == 0)
                break;

            if (parameterString.startsWith(PARAMETER_STRINGS[i]))
            {
                uint8_t substringStart = i == FW ? 3 : strlen(PARAMETER_STRINGS[i]); // need to handle FW differently as it is followed by a colon
                int index = parameterString.indexOf(' ');
                if (index > substringStart)
                {
                    command->parameters[i] = parameterString.substring(substringStart, index);
                    parameterString = parameterString.substring(index);
                }
                else {
                    command->parameters[i] = parameterString.substring(substringStart);
                    break;
                }
            }
        }
    }

    CommandType TouchDisplay::parseCommandType(const String commandString)
    {
        const char firstChar = commandString.charAt(0);

        if ('M' == firstChar || 'L' == firstChar)        // check proprietary commands first
            for (uint8_t i = 2; i < COMMANDS_COUNT; i++) // start at 2 to exclude Unknown and GCode
            {
                if (commandString.startsWith(COMMAND_STRINGS[i]))
                    return (CommandType)i;
            }

        if ('M' == firstChar || ('G' == firstChar && 'e' != commandString.charAt(1))) // filter out Geeetech
            return GCode;

        return Unknown;
    }

    void TouchDisplay::ignoreCommands(const bool ignore) { ignoreIncomingCommands = ignore; }

    String TouchDisplay::receiveCommandString()
    {
        String receivedLine = LCD_SERIAL.readStringUntil('\n');
        int lastStarChar = receivedLine.lastIndexOf('*');

        // just remove checksum without checking 8-0
        String result = receivedLine;
        if (lastStarChar > 0)
            result = receivedLine.substring(0, lastStarChar);

        // have to do "while" here because at least one command uses two times "N-0" :-(
        while (result.startsWith("N-0 "))
            result = result.substring(4);

        return result;
    }

    void TouchDisplay::sendToDisplay(PGM_P message, const bool addChecksum)
    {
        if (addChecksum)
        {
            LCD_SERIAL.write("N-0 ");

            uint8_t checksum = 115; // checksum of line sent above
            for (unsigned int i = 0; i < strlen(message); i++)
            {
                checksum ^= message[i];
                LCD_SERIAL.write(message[i]);
            }
            char number[5];
            sprintf(number, "*%d", checksum);
            LCD_SERIAL.write(number);
        }
        else
        {
            LCD_SERIAL.write(message);
        }

        LCD_SERIAL.write("\r\n");
    }

    void TouchDisplay::handleGcode(const String *gcode)
    {
#ifdef GEEETECH_DISPLAY_DEBUG
        SERIAL_ECHOLNPGM("Queueing command: ", gcode->c_str());
#endif
        queue.enqueue_now_P(PSTR(gcode->c_str()));
    }

    void TouchDisplay::handleUnkownCommand(const UiCommand *command)
    {
#ifdef GEEETECH_DISPLAY_DEBUG
        SERIAL_ECHOLNPGM("Unknown command ignored!");
#endif
    }

    void TouchDisplay::handleProprietaryCommand(const UiCommand *command)
    {
        if (M2120 == command->type)
        {
            handleM2120(command);
        }
    }

    void TouchDisplay::handleM2120(const UiCommand *command)
    {
        switch (command->parameters[P].charAt(0))
        {
        case '0':
            simulatedAutoLevelSwitchOn = '1' == command->parameters[S].charAt(0);
            SERIAL_ECHOLNPGM("Switch: ", simulatedAutoLevelSwitchOn);
            break;
        default:
#ifdef GEEETECH_DEBUG_DISPLAY
            SERIAL_ECHOLNPGM("Did not handle M2120 with P", command->parameters[P].charAt(0));
#endif
            break;
        }
    }

    void TouchDisplay::sendL1AxisInfo()
    {
        char x[8], y[8], z[8]; // 6 digits + dot + \0

        dtostrf(getAxisPosition_mm(X), 0, 3, x);
        dtostrf(getAxisPosition_mm(Y), 0, 3, y);
        dtostrf(getAxisPosition_mm(Z), 0, 3, z);

        char output[10 + 3 * 7 + 3 + 1]; // 10 chars + 3*7 XYZ + 3 F + \0
        sprintf(output, "L1 X%s Y%s Z%s F%d",
                x /*7*/, y /*7*/, z /*7*/, FEEDRATE /*3*/);
        sendToDisplay(PSTR(output));
    }

    void TouchDisplay::sendL2TempInfo()
    {
        char bedCurrentTemp[6], bedTargetTemp[6], // 4 digits + dot + \0
            e0CurrentTemp[6], e0TargetTemp[6];

        dtostrf(getActualTemp_celsius(BED), 0, 1, bedCurrentTemp);
        dtostrf(getTargetTemp_celsius(BED), 0, 1, bedTargetTemp);
        dtostrf(getActualTemp_celsius(E0), 0, 1, e0CurrentTemp);
        dtostrf(getTargetTemp_celsius(E0), 0, 1, e0TargetTemp);

        char output[54 + 8 * 5 + 5 * 1 + 3 * 3 + 1];
        sprintf(output, "L2 B:%s /%s /%d T0:%s /%s /%d T1:%s /%s /%d T2:%s /%s /%d SD:%d F0:%d F2:50 R:%d FR:%d",
                bedCurrentTemp /*5*/, bedTargetTemp /*5*/, BED_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                SD_ACTIVE /*1*/, F0_SPEED /*3*/, PRINT_SPEED /*3*/, FEEDRATE /*3*/);

        sendToDisplay(PSTR(output));
    }

    uint32_t TouchDisplay::getMixerRatio()
    {
        uint32_t result = mixer.mix[2];
        result = (result << 7) + mixer.mix[1];
        result = (result << 7) + mixer.mix[0];
        return result;
    }

    uint8_t TouchDisplay::getPrintStatus()
    {
        if (printJobOngoing() || isPrintingFromMedia())
            return PRINT_STATUS_PRINTING;
        else if (isPrintingPaused() || isPrintingFromMediaPaused())
            return PRINT_STATUS_PAUSED;
        else if (card.isFileOpen() && card.eof())
            return PRINT_STATUS_FINISHED;
        return PRINT_STATUS_IDLE;
    }

    void TouchDisplay::sendL3PrintInfo()
    {
        char output[59 + 3 * 1 + 2 * 3 + 7 + 12 + 6 + 1];
        sprintf(output, "L3 PS:%d VL:0 MT:%d FT:%d AL:%d ST:1 WF:0 MR:%ld FN:%s PG:%d TM:%ld LA:0 LC:0",
                getPrintStatus() /*1*/, MOTOR_TENSION_STATUS /*1*/, FILAMENT_SENSOR_STATUS /*3*/,
                simulatedAutoLevelSwitchOn /*1*/, getMixerRatio() /*7*/, CURRENT_FILENAME /*12*/,
                getProgress_percent() /*3*/, getProgress_seconds_elapsed() /*6*/);

        sendToDisplay(PSTR(output));
    }

    void TouchDisplay::sendStatusIfNeeded(const millis_t *currentTimeMs)
    {
        if (ELAPSED(currentTimeMs, nextStatusSend))
        {
            sendL1AxisInfo();
            delay(20);
            sendL2TempInfo();
            delay(20);
            sendL3PrintInfo();
            delay(20);
            nextStatusSend = *currentTimeMs + STATUS_CYCLE_IN_MS;
        }
    }
} // namespace Geeetech

#endif // GEEETECH_A30T_TFT