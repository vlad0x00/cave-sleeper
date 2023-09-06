#include "sensors.hpp"
#include "config.hpp"
#include "print.hpp"

#include <SparkFun_TMP117.h>

namespace cvslpr {

static TMP117 tempsensor;

bool
init_sensors()
{
  return true;
  // if (tempsensor.begin()) {
  //   msg_println(F("Temperature sensor initialized."));
  //   return true;
  // }
  // msg_println(F("Failed to initialize temperature sensor."));
  // return false;
}

SensorsReadout
measure()
{
  // Replace the following lines with actual sensor readouts
  const double temp = 0.25; // tempsensor.readTempC();
  const double hum = 0.5;

  msg_println(F("Measurements taken."));
  msg_print(F("Temperature = "));
  msg_print(temp);
  msg_println("C");
  msg_print(F("Humidity = "));
  msg_println(hum);

  return SensorsReadout{ temp, hum };
}

} // namespace cvslpr
