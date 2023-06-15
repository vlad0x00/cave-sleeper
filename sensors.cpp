#include "sensors.hpp"

#include "print.hpp"

#include <SparkFun_TMP117.h>
#include <Wire.h>

namespace cvslpr {

TMP117 tempsensor;

bool
init_sensors()
{
  Wire.begin();

  if (tempsensor.begin()) {
    msg_println(F("Temperature sensor initialized."));
    return true;
  }
  msg_println(F("Failed to initialize temperature sensor."));
  return false;
}

SensorsReadout
measure(const DateTime& now)
{
  // Replace the following lines with actual sensor readouts
  const double temp = tempsensor.readTempC();
  // const double hum = 0.5;

  msg_println(F("Measurements taken."));
  msg_print(F("Temperature = "));
  msg_print(temp);
  msg_println("C");
  // msg_print(F("Humidity = "));
  // msg_println(hum);


}

} // namespace cvslpr
