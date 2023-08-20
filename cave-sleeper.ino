#include "bluetooth.hpp"
#include "config.hpp"
#include "i2c.hpp"
#include "led.hpp"
#include "log.hpp"
#include "print.hpp"
#include "rtc.hpp"
#include "sensors.hpp"
#include "sleep.hpp"

#include <Arduino.h>

namespace cvslpr {

// Set to false if initialization or a component fails.
// If false, the main loop executes nothing.
static bool status_good = true;

void
setup()
{
  delay(STARTUP_DELAY);

  // Init print first so that we can inform the user of any errors
  if (!init_print()) {
    status_good = false;
    return;
  }

  // Init basic functionalities
  if (!init_i2c() || !init_sleep()) {
    status_good = false;
    msg_println(F("Initialization failed."));
    return;
  }

  // Should RTC be initialized?
  if constexpr (INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    init_rtc_time();
  }

  // Init the rest of the components
  if constexpr (!INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    if (!(init_log() && init_rtc() && init_sensors() && init_bluetooth())) {
      status_good = false;
      msg_println(F("Initialization failed."));
      led_signal_error_perpetual();
    }
    msg_println(F("Initialization successful."));
    led_signal_good();
  } else {
    go_sleep(true);
  }

  rtc_wakeup = true;
}

void
loop()
{
  if constexpr (!INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    if (status_good) {
      if (bluetooth_wakeup) {
        bluetooth_transfer_data();
        bluetooth_wakeup = false;
      }
      if (rtc_wakeup) {
        const auto now = get_current_time();
        const auto readout = measure(now);
        log(readout, now);
        set_alarm_time(now);
        rtc_wakeup = false;
      }

      go_sleep(!bluetooth_wakeup && !rtc_wakeup);
    }
  }
}

} // namespace cvslpr

void
setup()
{
  cvslpr::setup();
}

void
loop()
{
  cvslpr::loop();
}
