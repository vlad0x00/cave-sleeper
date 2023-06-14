#include "bluetooth.hpp"

#include "config.hpp"
#include "utils.hpp"
#include "log.hpp"

namespace cvslpr {

SoftwareSerial bluetooth(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);
bool bluetooth_wakeup = false;

void
on_bluetooth_wakeup()
{
  msg_println(F("Woken up by bluetooth."));
  bluetooth_wakeup = true;
}

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
bluetooth_transfer_data() {
  for (;;) {
    const auto b = 0; // Should open a new file and use logfile only for writing
    if (b < 0) { break; }
    bluetooth.write(b);
  }
}

} // namespace cvslpr
