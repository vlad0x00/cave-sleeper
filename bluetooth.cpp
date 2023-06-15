#include "bluetooth.hpp"
#include "print.hpp"
#include "log.hpp"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SD.h>

namespace cvslpr {

static SoftwareSerial bluetooth(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);
bool bluetooth_wakeup = false;

bool
init_bluetooth() {
  pinMode(BLUETOOTH_INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_INTERRUPT_PIN), on_bluetooth_wakeup, RISING);

  bluetooth.begin(BLUETOOTH_SERIAL_BAUD_RATE);
  bluetooth.write("AT");

  delay(200);

  const auto o = bluetooth.read();
  const auto k = bluetooth.read();
  if (o != 'O' || k != 'K') {
    msg_println(F("Bluetooth not initialized."));
    return false;
  }

  return true;
}

void
on_bluetooth_wakeup()
{
  msg_println(F("Woken up by bluetooth."));
  bluetooth_wakeup = true;
}

void
bluetooth_transfer_data() {
  auto logfile = SD.open(LOG_FILENAME, FILE_READ);
  for (;;) {
    const auto b = logfile.read();
    if (b < 0) { break; }
    bluetooth.write(b);
  }
  logfile.close();
}

} // namespace cvslpr
