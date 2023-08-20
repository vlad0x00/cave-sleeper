#include "led.hpp"

#include <Arduino.h>

namespace cvslpr {

constexpr inline int LED_PIN = 9;

bool
init_led()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  return true;
}

void
led_signal_good()
{
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  digitalWrite(LED_PIN, LOW);
}

void
led_signal_error_perpetual()
{
  for (;;) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}

} // namespace cvslpr
