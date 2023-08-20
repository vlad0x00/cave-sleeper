#ifndef CAVE_SLEEPER_LED_HPP
#define CAVE_SLEEPER_LED_HPP

namespace cvslpr {

[[nodiscard]] bool
init_led();

void
led_signal_good();
void
led_signal_error_perpetual();

} // namespace cvslpr

#endif
