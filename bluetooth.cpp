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

static bool
test_bluetooth()
{
  // We need to turn off the bluetooth module to change the mode
  digitalWrite(BLUETOOTH_ON_PIN, LOW);
  digitalWrite(BLUETOOTH_KEY_PIN, HIGH);
  delay(300);
  // Turn on the bluetooth module
  digitalWrite(BLUETOOTH_ON_PIN, HIGH);

  bluetooth.begin(BLUETOOTH_SERIAL_BAUD_RATE_CMD);

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

  // Turn the module off again to revert to data mode
  digitalWrite(BLUETOOTH_ON_PIN, LOW);
  digitalWrite(BLUETOOTH_KEY_PIN, LOW);
  delay(300);
  // Turn on the bluetooth module
  digitalWrite(BLUETOOTH_KEY_PIN, HIGH);

  return true;
}

bool
init_bluetooth()
{
  pinMode(BLUETOOTH_KEY_PIN, OUTPUT);
  pinMode(BLUETOOTH_ON_PIN, OUTPUT);

  if (!test_bluetooth()) {
    msg_println(F("Bluetooth module not responding."));
    return false;
  }

  bluetooth.begin(BLUETOOTH_SERIAL_BAUD_RATE_DATA);
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
    bluetooth.write((const uint8_t*)(formatted_entry.data),
                    formatted_entry.size);
  }
  bluetooth.flush();

  logfile.close();
}

} // namespace cvslpr
