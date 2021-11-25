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
 * lcd/extui/geeetech_a30t/geeetech_a30t_send.cpp
 *
 * This handles sending status to the display
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
    millis_t TouchDisplay::nextStatusSend = 0;
    char TouchDisplay::output[] = {};

    void TouchDisplay::setNextSendMs(const millis_t &currentTimeMs) { nextStatusSend = currentTimeMs; }

    void TouchDisplay::sendStatusIfNeeded(const millis_t &currentTimeMs)
    {
        if (ELAPSED(currentTimeMs, nextStatusSend))
        {
            send_L1_AxisInfo();
            send_L2_TempInfo();
            send_L3_PrintInfo();
            nextStatusSend = currentTimeMs + SEND_CYCLE_IN_MS;
        }
    }

    void TouchDisplay::send_L1_AxisInfo()
    {
        char x[8], y[8], z[8]; // 6 digits + dot + \0

        dtostrf(getAxisPosition_mm(X), 0, 3, x);
        dtostrf(getAxisPosition_mm(Y), 0, 3, y);
        dtostrf(getAxisPosition_mm(Z), 0, 3, z);

        sprintf(output, "L1 X%s Y%s Z%s F%d",
                x /*7*/, y /*7*/, z /*7*/, FEEDRATE /*3*/);
        sendToDisplay(output);
    }

    void TouchDisplay::send_L2_TempInfo()
    {
        sprintf(output, "L2 B:%s /%s /%d T0:%s /%s /%d T1:%s /%s /%d T2:%s /%s /%d SD:%d F0:%d F2:50 R:%d FR:%d",
                bedCurrentTemp /*5*/, bedTargetTemp /*5*/, BED_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                e0CurrentTemp /*5*/, e0TargetTemp /*5*/, E0_ACTIVE /*1*/,
                SD_ACTIVE /*1*/, F0_SPEED /*3*/, PRINT_SPEED /*3*/, FEEDRATE /*3*/);

        sendToDisplay(output);
    }

    void TouchDisplay::send_L3_PrintInfo()
    {
        sprintf(output, "L3 PS:%d VL:0 MT:%d FT:%d AL:%d ST:1 WF:0 MR:%ld FN:%s PG:%d TM:%ld LA:0 LC:0",
                getPrintStatus() /*1*/, MOTOR_TENSION_STATUS /*1*/, FILAMENT_SENSOR_STATUS /*3*/,
                simulatedAutoLevelSwitchOn /*1*/, getMixerRatio() /*7*/, CURRENT_FILENAME /*12*/,
                getProgress_percent() /*3*/, getProgress_seconds_elapsed() /*6*/);

        sendToDisplay(output);
    }

    void TouchDisplay::send_L9_FirmwareInfo()
    {
        sprintf(output, "L9 DN:Geeetech;DM:A30T;SN:TheThomasD;FV:%s;PV:320.00 x 320.00 x 420.00;HV:GTM32_103_V1;", getFirmwareName_str());

        sendToDisplay(output);
    }

    void TouchDisplay::send_L10_ZOffset()
    {
        char zOffset[7];

        dtostrf(home_offset[Z_AXIS], 0, 2, zOffset);

        sprintf(output, "L10 S%s", zOffset);

        sendToDisplay(output);
    }

    void TouchDisplay::send_L11_ProbeZOffset()
    {
        char probeZOffset[7];

        dtostrf(getZOffset_mm(), 0, 2, probeZOffset);

        sprintf(output, "L11 P0 S%s", probeZOffset);

        sendToDisplay(output);
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
        delay(20);
    }

} // namespace Geeetech

#endif // GEEETECH_A30T_TFT