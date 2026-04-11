#ifndef WICO_PINS_H
#define WICO_PINS_H

#include <Arduino.h>

namespace Pins {

// GPIO2 is a strapping pin on ESP32-C3. Using it for "Up" could block boot if
// the joystick is held during reset, so version 1 uses GPIO7 instead.
static constexpr uint8_t kUp = 7;
static constexpr uint8_t kDown = 3;
static constexpr uint8_t kLeft = 4;
static constexpr uint8_t kRight = 5;
static constexpr uint8_t kFire = 6;

}  // namespace Pins

#endif
