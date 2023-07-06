#ifndef CAVE_SLEEPER_SENSORS_HPP
#define CAVE_SLEEPER_SENSORS_HPP

namespace cvslpr {

struct SensorsReadout
{
  double temp = 0.0;
  double hum = 0.0;
};

[[nodiscard]] bool
init_sensors();

SensorsReadout
measure();

} // namespace cvslpr

#endif
