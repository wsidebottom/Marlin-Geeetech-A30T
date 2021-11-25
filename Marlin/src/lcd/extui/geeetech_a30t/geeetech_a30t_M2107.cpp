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
 * lcd/extui/geeetech_a30t/geeetech_a30t_M2134.cpp
 *
 * This file implements the M2107 command for controlling manual leveling
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
    void TouchDisplay::handle_M2107_ManualLeveling(const UiCommand &command)
    {
        if (command.parameters[S].equals("10")) // move Z down 0.05mm
            moveUpDownSmallBigStep(MANUAL_LEVELING_MOVE_DIRECTION_UP, MANUAL_LEVELING_MOVE_SMALL_STEP);
        else
            switch (command.parameters[S].charAt(0))
            {
            case '0': // start
                handle_M2107_S0_Start();
                break;
            case '1': // move to pos 1 (RR)
                moveToXYWithZHop(levelRR_xPosString, levelRR_yPosString);
                break;
            case '2': // move to pos 2 (RL)
                moveToXYWithZHop(levelRL_xPosString, levelRL_yPosString);
                break;
            case '3': // move to pos 3 (FL)
                moveToXYWithZHop(levelFL_xPosString, levelFL_yPosString);
                break;
            case '4': // move to pos 4 (FR)
                moveToXYWithZHop(levelFR_xPosString, levelFR_yPosString);
                break;
            case '5': // move to pos 5 (C)
                moveToXYWithZHop(levelCenter_xPosString, levelCenter_yPosString);
                break;
            case '6': // move Z up 0.5mm
                moveUpDownSmallBigStep(MANUAL_LEVELING_MOVE_DIRECTION_UP, MANUAL_LEVELING_MOVE_BIG_STEP);
                break;
            case '7': // move Z down 0.5mm
                moveUpDownSmallBigStep(MANUAL_LEVELING_MOVE_DIRECTION_DOWN, MANUAL_LEVELING_MOVE_BIG_STEP);
                break;
            case '8': // OK / save
                handle_M2107_S8_OkSave();
                break;
            case '9': // move Z down 0.05mm
                moveUpDownSmallBigStep(MANUAL_LEVELING_MOVE_DIRECTION_DOWN, MANUAL_LEVELING_MOVE_SMALL_STEP);
                break;
            }

        send_L10_ZOffset();
    }

    void TouchDisplay::handle_M2107_S0_Start()
    {
        simulatedAutoLevelSwitchOn = false;
        if (!isMachineHomed())
            handleGcode("G28");
    };

    void TouchDisplay::handle_M2107_S8_OkSave()
    {
        simulatedAutoLevelSwitchOn = true;
        handleGcode("M500\nG91\nG0 Z10\nG90\nG0 X0 Y0 F1000"); // store settings and return home
        sendToDisplay("M2107 save success")
    };

    void TouchDisplay::moveToXYWithZHop(const char *xPos, const char *yPos)
    {
        String gcode = "G0 Z" + MANUAL_LEVELING_MOVE_Z_HOP + "\nG0 X%s Y%s\nG0 Z-" + MANUAL_LEVELING_MOVE_Z_HOP;
        sprintf(output, gcode.c_str(), xPos, yPos);
        handleGcode(output);
        sendToDisplay("M2107 ok");
    }

    void TouchDisplay::moveUpDownSmallBigStep(const bool &directionUpDown, const bool &bigSmallStep)
    {
        float newOffset = home_offset[Z_AXIS];

        if (directionUpDown == MANUAL_LEVELING_MOVE_DIRECTION_UP)
            newOffset -= bigSmallStep == MANUAL_LEVELING_MOVE_BIG_STEP ? 0.5 : 0.05;
        else
            newOffset += bigSmallStep == MANUAL_LEVELING_MOVE_BIG_STEP ? 0.5 : 0.05;

        set_home_offset(Z_AXIS, newOffset);

        handleGcode("G0 Z0 F300");
    }
} // namespace Geeetech

#endif // GEEETECH_A30T_TFT