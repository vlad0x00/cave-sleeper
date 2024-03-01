#ifndef CAVE_SLEEPER_PRINT_HPP
#define CAVE_SLEEPER_PRINT_HPP

#include "config.hpp"

#include <Arduino.h>

namespace cvslpr {

[[nodiscard]] inline bool
init_print()
{
  if constexpr (PRINT_DEBUG) {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) {
    }
  }
  return true;
}

template<typename T>
inline void
msg_print(T t)
{
  if constexpr (PRINT_DEBUG) {
    Serial.print(t);
    Serial.flush();
  }
}

template<typename T>
inline void
msg_println(T t)
{
  if constexpr (PRINT_DEBUG) {
    Serial.println(t);
    Serial.flush();
  }
}

} // namespace cvslpr

#endif
