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

using namespace ExtUI;

// status values
#define PRINT_STATUS_IDLE 0
#define PRINT_STATUS_PRINTING 1
#define PRINT_STATUS_PAUSED 2
#define PRINT_STATUS_FINISHED 4

#define FILAMENT_SENSOR_DISABLED 255
#define FILAMENT_SENSOR_OUT 0
#define FILAMENT_SENSOR_IN 1

// state macros
#define FEEDRATE (uint8_t) round(ExtUI::getFeedrate_mm_s())
#define BED_ACTIVE thermalManager.isHeatingBed() ? 1 : 0
#define E0_ACTIVE thermalManager.isHeatingHotend(0) ? 1 : 0
#define SD_ACTIVE isMediaInserted() ? 0 : 1
#define F0_SPEED (uint8_t) round(getActualFan_percent(FAN0))
#define PRINT_SPEED (uint8_t) round(getFeedrate_percent())
#define FILAMENT_SENSOR_STATUS getFilamentRunoutEnabled() ? (getFilamentRunoutState() ? FILAMENT_SENSOR_IN : FILAMENT_SENSOR_OUT) : FILAMENT_SENSOR_DISABLED
#define CURRENT_FILENAME card.filename
#define MOTOR_TENSION_STATUS stepper.axis_is_enabled(X_AXIS) || stepper.axis_is_enabled(Y_AXIS) || stepper.axis_is_enabled(Z_AXIS) || stepper.axis_is_enabled(E_AXIS)

String receiveCommand()
{
    String receivedLine = LCD_SERIAL.readStringUntil('\n');
    int lastStarChar = receivedLine.lastIndexOf('*');

    // just remove checksum without checking :-0
    String result = receivedLine;
    if (lastStarChar > 0)
        result = receivedLine.substring(0, lastStarChar);

    // have to do "while" here because at least one command uses two times "N-0" :(
    while (result.startsWith("N-0 "))
        result = result.substring(4);

    return result;
}

bool canForwardToQueue(String command)
{
    bool result = false;

    if ('G' == command.charAt(0) && 'e' != command.charAt(1)) // exclude Geeetech
        result = true;                                        // G-based codes are OK
    else if ('M' == command.charAt(0) && command.indexOf(' ') <= 4)
        result = true; // M-based codes are OK if number is max 3 digits

    return result;
}

bool isProprietaryCommand(String command) {
    return false;
}

void sendToDisplay(PGM_P message, bool addChecksum)
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

void queueGcode(String gcode)
{
    queue.enqueue_now_P(PSTR(gcode.c_str()));
}

void sendL1AxisInfo()
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

void sendL2TempInfo()
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

uint32_t getMixerRatio()
{
    uint32_t result = mixer.mix[2];
    result = (result << 7) + mixer.mix[1];
    result = (result << 7) + mixer.mix[0];
    return result;
}

uint8_t getPrintStatus()
{
    if (printJobOngoing() || isPrintingFromMedia())
        return PRINT_STATUS_PRINTING;
    else if (isPrintingPaused() || isPrintingFromMediaPaused())
        return PRINT_STATUS_PAUSED;
    else if (card.isFileOpen() && card.eof())
        return PRINT_STATUS_FINISHED;
    return PRINT_STATUS_IDLE;
}

void sendL3PrintInfo()
{
    char output[60 + 2 * 1 + 2 * 3 + 7 + 12 + 6 + 1];
    sprintf(output, "L3 PS:%d VL:0 MT:%d FT:%d AL:1 ST:1 WF:0 MR:%ld FN:%s PG:%d TM:%ld LA:0 LC:0",
            getPrintStatus() /*1*/, MOTOR_TENSION_STATUS /*1*/, FILAMENT_SENSOR_STATUS /*3*/,
            getMixerRatio() /*7*/, CURRENT_FILENAME /*12*/, getProgress_percent() /*3*/,
            getProgress_seconds_elapsed() /*6*/);

    sendToDisplay(PSTR(output));
}

void sendStatus() {
    sendL1AxisInfo();
    delay(20);
    sendL2TempInfo();
    delay(20);
    sendL3PrintInfo();
    delay(20);
}

#endif // GEEETECH_A30T_TFT