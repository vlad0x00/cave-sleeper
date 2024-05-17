#ifndef CAVE_SLEEPER_SENSORS_HPP
#define CAVE_SLEEPER_SENSORS_HPP

namespace cvslpr {

struct SensorsReadout
{
  SensorsReadout(const double temp)
    : temp(temp)
  {
  }

  double temp = 0.0;
  // TODO: Other sensor readouts can be added here
  // and to the constructor parameter list.
};

[[nodiscard]] bool
init_sensors();

SensorsReadout
measure();

} // namespace cvslpr

#endif
