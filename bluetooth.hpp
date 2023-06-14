#ifndef CAVE_SLEEPER_BLUETOOTH_HPP
#define CAVE_SLEEPER_BLUETOOTH_HPP

#include <SoftwareSerial.h>

namespace cvslpr {

extern SoftwareSerial bluetooth;
extern bool bluetooth_wakeup;

void
on_bluetooth_wakeup();

bool
init_bluetooth();

void
bluetooth_transfer_data();

} // namespace cvslpr

#endif
