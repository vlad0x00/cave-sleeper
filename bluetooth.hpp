#ifndef CAVE_SLEEPER_BLUETOOTH_HPP
#define CAVE_SLEEPER_BLUETOOTH_HPP

namespace cvslpr {

constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE = 9600;
constexpr inline int BLUETOOTH_INTERRUPT_PIN = 3;
constexpr inline int BLUETOOTH_RX_PIN = 4;
constexpr inline int BLUETOOTH_TX_PIN = 5;

extern bool bluetooth_wakeup;

[[nodiscard]] bool
init_bluetooth();

void
on_bluetooth_wakeup();

void
bluetooth_transfer_data();

} // namespace cvslpr

#endif
