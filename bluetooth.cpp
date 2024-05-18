#include "bluetooth.hpp"
#include "config.hpp"
#include "log.hpp"
#include "print.hpp"

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>

namespace cvslpr {

static SoftwareSerial bluetooth(BLUETOOTH_TX_PIN, BLUETOOTH_RX_PIN);
volatile bool bluetooth_switch_interrupt_registered = false;

enum class BluetoothMode
{
  DATA,
  COMMAND
};

static const char* BLUETOOTH_MODE_NAMES[] = { "data", "command" };

static void
bluetooth_turn_on(const BluetoothMode mode)
{
  digitalWrite(BLUETOOTH_KEY_PIN, mode == BluetoothMode::DATA ? LOW : HIGH);
  delay(300);
  bluetooth.begin(mode == BluetoothMode::DATA ? BLUETOOTH_SERIAL_BAUD_RATE_DATA
                                              : BLUETOOTH_SERIAL_BAUD_RATE_CMD);
  msg_print(F("Bluetooth turned on with "));
  msg_print(BLUETOOTH_MODE_NAMES[static_cast<int>(mode)]);
  msg_print(F(" mode."));
}

static void
on_bluetooth_switch_interrupt()
{
  msg_println(F("Bluetooth switch interrupt registered."));
  bluetooth_switch_interrupt_registered = true;
}

static bool
wait_for_connection()
{
  msg_println(F("Waiting for bluetooth connection..."));
  bool connected = false;
  for (;;) {
    if (digitalRead(BLUETOOTH_STATE_PIN) == HIGH) {
      msg_println(F("Bluetooth connected."));
      return true;
    }
    if (digitalRead(BLUETOOTH_SWITCH_INTERRUPT_PIN) == HIGH) {
      msg_println(F("Bluetooth switched off while waiting for connection."));
      return false;
    }
  }
}

static void
wait_for_off()
{
  msg_println(F("Waiting for bluetooth module to be turned off..."));
  while (digitalRead(BLUETOOTH_SWITCH_INTERRUPT_PIN) == HIGH) {
  }
  // The delay prevents multiple interrupt triggers being handled
  delay(200);
}

static void
bluetooth_transfer_data()
{
  auto logfile = open_log();
  if (!logfile) {
    bluetooth.write(F("Failed to open log file."));
    return;
  }

  msg_println(F("Transferring data..."));
  bluetooth.write(FormattedLogEntry::HEADER);
  LogEntry entry;
  while (load_log_entry(logfile, entry)) {
    FormattedLogEntry formatted_entry(entry);
    if constexpr (PRINT_DEBUG) {
      const auto written = bluetooth.write(
        (const uint8_t*)(formatted_entry.data), formatted_entry.size);
      if (written != formatted_entry.size) {
        msg_println(F("Failed to write all data to bluetooth."));
      }
    } else {
      bluetooth.write((const uint8_t*)(formatted_entry.data),
                      formatted_entry.size);
    }
  }
  bluetooth.flush();

  logfile.close();
  msg_println(F("Data transfer done."));
}

bool
init_bluetooth()
{
  pinMode(BLUETOOTH_KEY_PIN, OUTPUT);
  pinMode(BLUETOOTH_STATE_PIN, INPUT);
  pinMode(BLUETOOTH_SWITCH_INTERRUPT_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_SWITCH_INTERRUPT_PIN),
                  on_bluetooth_switch_interrupt,
                  RISING);

  msg_println(F("Bluetooth initialized."));
  return true;
}

void
bluetooth_handle_transfer()
{
  bluetooth_turn_on(BluetoothMode::DATA);
  if (wait_for_connection()) {
    bluetooth_transfer_data();
    wait_for_off();
  }
}

} // namespace cvslpr
