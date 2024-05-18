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

  if (!init_led()) {
    status_good = false;
    return;
  }

  // Init print first so that we can inform the user of any errors
  if (!init_print()) {
    status_good = false;
    led_signal_error_perpetual();
  }

  // Init
  if (!init_i2c() || !init_sleep() || !init_log() || !init_rtc() ||
      !init_sensors() || !init_bluetooth()) {
    status_good = false;
    msg_println(F("===========================\nInitialization "
                  "failed.\n==========================="));
    led_signal_error_perpetual();
  }
  msg_println(F("===========================\nInitialization "
                "successful.\n==========================="));
  led_signal_good();

  // Should RTC be initialized?
  if constexpr (INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    init_rtc_time();
  }

  // If we just wanted to init the RTC, go sleep
  if constexpr (INIT_RTC_TIME && !INIT_RTC_TIME_AND_RUN) {
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
        const auto readout = measure();
        log(readout, now);
        if (!set_alarm_time(now)) {
          status_good = false;
          msg_println(F("Setting alarm failed."));
          led_signal_error_perpetual();
        }
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
