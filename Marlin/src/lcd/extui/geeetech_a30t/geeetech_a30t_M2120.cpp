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
 * lcd/extui/geeetech_a30t/geeetech_a30t_M2120.cpp
 *
 * This file implements the M2120 command controling the autoleveling
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
    void TouchDisplay::handleM2120(const UiCommand &command)
    {
        switch (command.parameters[P].charAt(0))
        {
        case '0':
            simulatedAutoLevelSwitchOn = '1' == command.parameters[S].charAt(0);
            break;
        case '1':
            handleM2120P1(command.parameters[S].charAt(0));
            break;
        default:
#ifdef GEEETECH_DISPLAY_DEBUG
            SERIAL_ECHOLNPGM("Did not handle M2120 with P", command.parameters[P].charAt(0));
#endif
            break;
        }
    }

    void TouchDisplay::handleM2120P1(const char &sParameter)
    {
        // BLtouch control
        switch (sParameter)
        {
        case '0':                // pin up
            handleGcode("M402"); // stow probe
            break;
        case '1':                // pin down
            handleGcode("M401"); // deploy probe
            break;
        default:                         // alarm release ('2')
            handleGcode("M280 P0 S160"); // control servo
            break;
        }
    }
} // namespace Geeetech

#endif // GEEETECH_A30T_TFT