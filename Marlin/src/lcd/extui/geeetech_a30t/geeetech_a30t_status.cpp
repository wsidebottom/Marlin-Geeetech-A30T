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
 * lcd/extui/geeetech_a30t/geeetech_a30t_status.cpp
 *
 * This handles providing and collecting status information
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
    millis_t TouchDisplay::nextTempDataUpdate = 0;
    char TouchDisplay::bedCurrentTemp[] = {};
    char TouchDisplay::bedTargetTemp[] = {};
    char TouchDisplay::e0CurrentTemp[] = {};
    char TouchDisplay::e0TargetTemp[] = {};

    void TouchDisplay::updateTempDataIfNeeded(const millis_t *currentTimeMs)
    {
        if (ELAPSED(*currentTimeMs, nextTempDataUpdate))
        {
            dtostrf(getActualTemp_celsius(BED), 0, 1, bedCurrentTemp);
            dtostrf(getTargetTemp_celsius(BED), 0, 1, bedTargetTemp);
            dtostrf(getActualTemp_celsius(E0), 0, 1, e0CurrentTemp);
            dtostrf(getTargetTemp_celsius(E0), 0, 1, e0TargetTemp);

            nextTempDataUpdate = *currentTimeMs + TEMP_STATUS_CYCLE_IN_MS;
        }
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

} // namespace Geeetech

#endif // GEEETECH_A30T_TFT