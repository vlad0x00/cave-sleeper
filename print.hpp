#ifndef CAVE_SLEEPER_PRINT_HPP
#define CAVE_SLEEPER_PRINT_HPP

#include <Arduino.h>

namespace cvslpr {

// Change to true to print debug messages to serial port.
constexpr inline bool PRINT_DEBUG = true;

constexpr inline long SERIAL_BAUD_RATE = 9600;

[[nodiscard]] inline bool
init_print()
{
  Serial.begin(SERIAL_BAUD_RATE);
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
