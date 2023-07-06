#ifndef CAVE_SLEEPER_BLUETOOTH_HPP
#define CAVE_SLEEPER_BLUETOOTH_HPP

namespace cvslpr {

extern bool bluetooth_wakeup;

[[nodiscard]] bool
init_bluetooth();

void
on_bluetooth_wakeup();

void
bluetooth_transfer_data();

} // namespace cvslpr

#endif
