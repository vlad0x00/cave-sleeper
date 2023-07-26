#ifndef CAVE_SLEEPER_SLEEP_HPP
#define CAVE_SLEEPER_SLEEP_HPP

#include "print.hpp"

#include <avr/sleep.h>

namespace cvslpr {

[[nodiscard]] bool
init_sleep();

// Necessary order of instructions in order to avoid race conditions.
// See <avr/sleep.h> for more information.
#define go_sleep(condition)                                                    \
  cli();                                                                       \
  if (condition) {                                                             \
    msg_println(F("Sleeping..."));                                             \
    sleep_enable();                                                            \
    sei();                                                                     \
    sleep_cpu();                                                               \
    sleep_disable();                                                           \
  }                                                                            \
  sei();

} // namespace cvslpr

#endif
