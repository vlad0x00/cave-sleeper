#ifndef CAVE_SLEEPER_SENSORS_HPP
#define CAVE_SLEEPER_SENSORS_HPP

#include <DS3231.h>

namespace cvslpr {

struct SensorsReadout
{
  SensorsReadout(const double temp, const double hum)
    : temp(temp)
    , hum(hum)
  {
  }

  double temp = 0.0;
  double hum = 0.0;
};

[[nodiscard]] bool
init_sensors();

SensorsReadout
measure(const DateTime& now);

} // namespace cvslpr

#endif
