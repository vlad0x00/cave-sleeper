#include "bluetooth.hpp"
#include "config.hpp"
#include "log.hpp"
#include "print.hpp"

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>

namespace cvslpr {

static SoftwareSerial bluetooth(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);
volatile bool bluetooth_wakeup = false;

enum class BluetoothMode
{
  DATA,
  COMMAND
};

static void
bluetooth_mode(const BluetoothMode mode)
{
  digitalWrite(BLUETOOTH_ON_PIN, LOW);
  digitalWrite(BLUETOOTH_KEY_PIN, mode == BluetoothMode::DATA ? LOW : HIGH);
  delay(300);
  digitalWrite(BLUETOOTH_ON_PIN, HIGH);
  bluetooth.begin(mode == BluetoothMode::DATA ? BLUETOOTH_SERIAL_BAUD_RATE_DATA
                                              : BLUETOOTH_SERIAL_BAUD_RATE_CMD);
}

static bool
test_bluetooth()
{
  // Check if the module responds
  bluetooth.write(F("AT\r\n"));
  bluetooth.flush();

  // Let the bluetooth module respond
  delay(500);

  // Ensure that the response is "OK"
  if (!bluetooth.available() || bluetooth.read() != 'O' ||
      !bluetooth.available() || bluetooth.read() != 'K') {
    return false;
  }
  // Discard the rest of the response
  while (bluetooth.available()) {
    bluetooth.read();
  }

  return true;
}

bool
init_bluetooth()
{
  pinMode(BLUETOOTH_KEY_PIN, OUTPUT);
  pinMode(BLUETOOTH_ON_PIN, OUTPUT);

  if constexpr (BLUETOOTH_TEST) {
    if (!test_bluetooth()) {
      bluetooth_mode(BluetoothMode::COMMAND);
      msg_println(F("Bluetooth module not responding."));
      return false;
    }
  }

  bluetooth_mode(BluetoothMode::DATA);

  pinMode(BLUETOOTH_INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_INTERRUPT_PIN),
                  on_bluetooth_wakeup,
                  RISING);

  msg_println(F("Bluetooth module initialized."));
  return true;
}

void
on_bluetooth_wakeup()
{
  msg_println(F("Woken up by bluetooth."));
  bluetooth_wakeup = true;
}

void
bluetooth_transfer_data()
{
  auto logfile = open_log();
  if (!logfile) {
    bluetooth.write(F("Failed to open log file."));
    return;
  }

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
}

} // namespace cvslpr
