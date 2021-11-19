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
 * lcd/extui/geeetech_a30t/geeetech_a30t_extui.cpp
 */

#include "../../../inc/MarlinConfigPre.h"

#if ENABLED(GEEETECH_A30T_TFT)

#include "../ui_api.h"
#include "geeetech_a30t.h"

namespace ExtUI
{
  void onStartup()
  {
    // use pin PD12 (60) as output and pull it up to supply 5V power
    SET_OUTPUT(60);
    WRITE(60, 1);

    // setup the communication with the display   
    LCD_SERIAL.setRx(LCD_UART_RX); // use PD9 for RX
    LCD_SERIAL.setTx(LCD_UART_TX); // use PD8 for TX
    LCD_SERIAL.begin(115200); // Geeetech uses 115200 speed

    #ifdef MYSERIAL1
    MYSERIAL1.println("Geeetech A30T TFT initialized");
    #endif
  }

  void onIdle()
  {
    while (LCD_SERIAL.available()) {
      String command = receiveCommand();

      if (canForwardToQueue(command))
        queueGcode(command);
      #ifdef MYSERIAL1
      else
        MYSERIAL1.println(("Cannot queue: " + command).c_str());
      #endif
    }
  }

  void onFilamentRunout(const extruder_t extruder) {}

  void onPrinterKilled(FSTR_P const error, FSTR_P const component) {}

  void onPrintTimerStarted() {}
  void onPrintTimerPaused() {}
  void onPrintTimerStopped() {}
  void onPrintFinished() {}

  void onStatusChanged(const char * const msg) {}

  void onMediaInserted() {}
  void onMediaError() {}
  void onMediaRemoved() {}

  void onPlayTone(const uint16_t, const uint16_t) {}

  void onUserConfirmRequired(const char * const msg) {};

  void onHomingStart() {}
  void onHomingComplete() {}
  
  void onFactoryReset() {}
  void onStoreSettings(char *) {}
  void onLoadSettings(const char *) {}
  void onPostprocessSettings() {}
  void onConfigurationStoreWritten(bool) {}
  void onConfigurationStoreRead(bool) {}

  void onMeshLevelingStart() {}
  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const_float_t zval) {}
  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const ExtUI::probe_state_t state) {}

  void onPowerLossResume() {}

  void onSteppersDisabled() {}
  void onSteppersEnabled() {}

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {}
  #endif
  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {}
  #endif
}

#endif // GEEETECH_A30T_TFT
