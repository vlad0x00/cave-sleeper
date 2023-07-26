#include "bluetooth.hpp"
#include "log.hpp"
#include "print.hpp"
#include "rtc.hpp"
#include "sensors.hpp"
#include "sleep.hpp"

#include <Arduino.h>

namespace cvslpr {

// How long to wait after booting before running any code. Useful in order to
// avoid code execution on boot in case you instead want to upload different
// code.
constexpr inline unsigned STARTUP_DELAY = 5000;

// Set to false if initialization or a component fails.
// If false, the main loop executes nothing.
static bool status_good = true;

void
setup()
{
  delay(STARTUP_DELAY);

  if (!init_print()) {
    status_good = false;
    return;
  }
  if (!init_sleep()) {
    status_good = false;
    msg_println(F("Initialization failed."));
    return;
  }

  if constexpr (INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    init_rtc_time();
  }

  if constexpr (!INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    if (!(init_log() && init_rtc() && init_sensors() && init_bluetooth())) {
      status_good = false;
      msg_println(F("Initialization failed."));
      go_sleep(true);
    }
    msg_println(F("Initialization successful."));
  } else {
    go_sleep(true);
  }

  noInterrupts();
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

      interrupts();

      go_sleep(!bluetooth_wakeup && !rtc_wakeup);

      noInterrupts();
    } else {
      interrupts();
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
