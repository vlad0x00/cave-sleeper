#ifndef CAVE_SLEEPER_LOG_HPP
#define CAVE_SLEEPER_LOG_HPP

#include "rtc.hpp"
#include "sensors.hpp"

namespace cvslpr {

[[nodiscard]] bool
init_log();

[[nodiscard]] bool
deinit_log();

void
log(const SensorsReadout& readout, const DateTime& now);

} // namespace cvslpr

#endif
