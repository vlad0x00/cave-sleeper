#include "sensors.hpp"
#include "config.hpp"
#include "print.hpp"

#include <SparkFun_TMP117.h>

namespace cvslpr {

static TMP117 tempsensor;

bool
init_sensors()
{
  // TODO: Add any other sensor init here
  if (tempsensor.begin()) {
    msg_println(F("Temperature sensor initialized."));
    return true;
  }
  msg_println(F("Failed to initialize temperature sensor."));
  return false;
}

SensorsReadout
measure()
{
  const double temp = tempsensor.readTempC();
  // TODO: Add any other sensor readouts here

  msg_println(F("Measurements taken."));
  msg_print(F("Temperature = "));
  msg_print(temp);
  msg_println(F("C"));
  // TODO: Also print any other readouts, e.g. humidity
  // msg_print(F("Humidity = "));
  // msg_println(hum);

  return SensorsReadout{ temp };
}

} // namespace cvslpr
