#ifndef CAVE_SLEEPER_SENSORS_HPP
#define CAVE_SLEEPER_SENSORS_HPP

#include "config.hpp"
#include "utils.hpp"

#include <SparkFun_TMP117.h>

namespace cvslpr {

TMP117 tempsensor;

static inline bool
init_sensors()
{
  if (tempsensor.begin()) {
    msg_println(F("Temperature sensor initialized."));
    return true;
  }
  msg_println(F("Failed to initialize temperature sensor."));
  return false;
}

} // namespace cvslpr

#endif
