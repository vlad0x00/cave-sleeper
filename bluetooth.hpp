#ifndef CAVE_SLEEPER_BLUETOOTH_HPP
#define CAVE_SLEEPER_BLUETOOTH_HPP

namespace cvslpr {

extern volatile bool bluetooth_switch_interrupt_registered;

[[nodiscard]] bool
init_bluetooth();

void
bluetooth_handle_transfer();

} // namespace cvslpr

#endif
