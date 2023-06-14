#ifndef CAVE_SLEEPER_UTILS_HPP
#define CAVE_SLEEPER_UTILS_HPP

#include "config.hpp"

#include <Arduino.h>

namespace cvslpr {

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
