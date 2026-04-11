#ifndef WICO_JOYSTICK_H
#define WICO_JOYSTICK_H

#include <Arduino.h>
#include <stdint.h>

struct JoystickState {
  bool up;
  bool down;
  bool left;
  bool right;
  bool fire;
};

struct JoystickAxes {
  int16_t x;
  int16_t y;
};

class JoystickInput {
 public:
  void begin();
  bool update();

  const JoystickState& state() const;
  JoystickAxes axes(int16_t minValue, int16_t centerValue, int16_t maxValue) const;

 private:
  enum InputIndex : uint8_t {
    kUpIndex,
    kDownIndex,
    kLeftIndex,
    kRightIndex,
    kFireIndex,
    kInputCount
  };

  struct DebouncedInput {
    uint8_t pin;
    const char* name;
    bool stablePressed;
    bool lastRawPressed;
    uint32_t lastChangeMs;
  };

  DebouncedInput inputs_[kInputCount];
  JoystickState state_;

  void configureInput(InputIndex index, uint8_t pin, const char* name);
  static bool isPressed(uint8_t pin);
  void rebuildState();
};

String formatJoystickState(const JoystickState& state);

#endif
