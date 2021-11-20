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
#define STATUS_CYCLE_IN_MS 1000
  millis_t nextStatusSend = 0;
  bool ignoreIncomingCommands = false;

  void onStartup()
  {
    // use pin PD12 (60) as output and pull it up to supply 5V power
    SET_OUTPUT(60);
    WRITE(60, 1);

    // setup the communication with the display
    LCD_SERIAL.setRx(LCD_UART_RX); // use PD9 for RX
    LCD_SERIAL.setTx(LCD_UART_TX); // use PD8 for TX
    LCD_SERIAL.begin(115200);      // Geeetech uses 115200 speed(

#ifdef MYSERIAL1
    MYSERIAL1.println("Geeetech A30T TFT initialized");
#endif
  }

#define MAX_RECEIVE_COMMANDS 10

  void onIdle()
  {
    const millis_t currentTimeMs = millis();
    if (ELAPSED(currentTimeMs, nextStatusSend))
    {
      sendStatus();
      nextStatusSend = currentTimeMs + STATUS_CYCLE_IN_MS;
    }

    String commands[MAX_RECEIVE_COMMANDS];
    int receivedCommands = 0;
    for (; receivedCommands < MAX_RECEIVE_COMMANDS && LCD_SERIAL.available(); receivedCommands++)
      commands[receivedCommands] = receiveCommand();

    if (!ignoreIncomingCommands) // commands during homing etc. will be lost
    {
      String proprietaryCommands[10];
      uint8_t proprietaryCommandIndex = 0;
      String commandsToQueue = "";
      String unknownCommands = "";
      for (uint8_t i = 0; i < receivedCommands; i++)
      {
        if (isProprietaryCommand(commands[i]))
          proprietaryCommands[proprietaryCommandIndex++] = commands[i];
        else if (canForwardToQueue(commands[i]))
          commandsToQueue += commands[i] + "\n";
        else
          unknownCommands += commands[i] + "\n";
        if (commands[i].startsWith("G28") || commands[i].startsWith("G29"))
          break;
      }
      if (commandsToQueue.length() > 0)
      {
        queueGcode(commandsToQueue.substring(0, commandsToQueue.length() - 1));
        nextStatusSend = currentTimeMs; // update status in next cycle
      }

#ifdef MYSERIAL1
      if (unknownCommands.length() > 0)
      {
        MYSERIAL1.write("Cannot queue: ");
        MYSERIAL1.write(unknownCommands.c_str());
      }
      MYSERIAL1.write("\n");
#endif
    }
  }

  void onFilamentRunout(const extruder_t extruder) {}

  void onPrinterKilled(FSTR_P const error, FSTR_P const component) {}

  void onPrintTimerStarted() {}
  void onPrintTimerPaused() {}
  void onPrintTimerStopped() {}
  void onPrintFinished() {}

  void onStatusChanged(const char *const msg) {}

  void onMediaInserted() {}
  void onMediaError() {}
  void onMediaRemoved() {}

  void onPlayTone(const uint16_t, const uint16_t) {}

  void onUserConfirmRequired(const char *const msg){};

  void onHomingStart() { ignoreIncomingCommands = true; }
  void onHomingComplete() { ignoreIncomingCommands = false; }

  void onFactoryReset() {}
  void onStoreSettings(char *) {}
  void onLoadSettings(const char *) {}
  void onPostprocessSettings() {}
  void onConfigurationStoreWritten(bool) {}
  void onConfigurationStoreRead(bool) {}

  void onMeshLevelingStart() {}
  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const_float_t zval) {}
  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const ExtUI::probe_state_t state)
  {
    ignoreIncomingCommands = ExtUI::probe_state_t::G29_FINISH == state;
  }

  void onPowerLossResume() {}

  void onSteppersDisabled() {}
  void onSteppersEnabled() {}

#if ENABLED(POWER_LOSS_RECOVERY)
  void onPowerLossResume()
  {
  }
#endif
#if HAS_PID_HEATING
  void onPidTuning(const result_t rst)
  {
  }
#endif
}

#endif // GEEETECH_A30T_TFT
