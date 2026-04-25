#ifndef WICO_PINS_H
#define WICO_PINS_H

#include <Arduino.h>

namespace Pins {

// GPIO2 is a strapping pin on ESP32-C3 and is avoided for switch inputs.
static constexpr uint8_t kUp = 4;
static constexpr uint8_t kDown = 3;
static constexpr uint8_t kLeft = 8;
static constexpr uint8_t kRight = 5;
static constexpr uint8_t kFire = 7;

}  // namespace Pins

#endif
