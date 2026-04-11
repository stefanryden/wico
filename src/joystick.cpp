#include "joystick.h"

#include "config.h"
#include "pins.h"

void JoystickInput::configureInput(InputIndex index, uint8_t pin, const char* name) {
  inputs_[index].pin = pin;
  inputs_[index].name = name;
  inputs_[index].stablePressed = false;
  inputs_[index].lastRawPressed = false;
  inputs_[index].lastChangeMs = 0;
}

bool JoystickInput::isPressed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

void JoystickInput::begin() {
  configureInput(kUpIndex, Pins::kUp, "UP");
  configureInput(kDownIndex, Pins::kDown, "DOWN");
  configureInput(kLeftIndex, Pins::kLeft, "LEFT");
  configureInput(kRightIndex, Pins::kRight, "RIGHT");
  configureInput(kFireIndex, Pins::kFire, "FIRE");

  const uint32_t now = millis();

  for (uint8_t index = 0; index < kInputCount; ++index) {
    pinMode(inputs_[index].pin, INPUT_PULLUP);

    const bool pressed = isPressed(inputs_[index].pin);
    inputs_[index].stablePressed = pressed;
    inputs_[index].lastRawPressed = pressed;
    inputs_[index].lastChangeMs = now;
  }

  rebuildState();
}

bool JoystickInput::update() {
  bool changed = false;
  const uint32_t now = millis();

  for (uint8_t index = 0; index < kInputCount; ++index) {
    DebouncedInput& input = inputs_[index];
    const bool rawPressed = isPressed(input.pin);

    if (rawPressed != input.lastRawPressed) {
      input.lastRawPressed = rawPressed;
      input.lastChangeMs = now;
    }

    if ((now - input.lastChangeMs) >= Config::kDebounceMs &&
        input.stablePressed != input.lastRawPressed) {
      input.stablePressed = input.lastRawPressed;
      changed = true;
    }
  }

  if (changed) {
    rebuildState();
  }

  return changed;
}

const JoystickState& JoystickInput::state() const {
  return state_;
}

JoystickAxes JoystickInput::axes(int16_t minValue, int16_t centerValue, int16_t maxValue) const {
  JoystickAxes axes{centerValue, centerValue};

  if (state_.left == state_.right) {
    axes.x = centerValue;
  } else if (state_.left) {
    axes.x = minValue;
  } else {
    axes.x = maxValue;
  }

  if (state_.up == state_.down) {
    axes.y = centerValue;
  } else if (state_.up) {
    axes.y = minValue;
  } else {
    axes.y = maxValue;
  }

  return axes;
}

void JoystickInput::rebuildState() {
  state_.up = inputs_[kUpIndex].stablePressed;
  state_.down = inputs_[kDownIndex].stablePressed;
  state_.left = inputs_[kLeftIndex].stablePressed;
  state_.right = inputs_[kRightIndex].stablePressed;
  state_.fire = inputs_[kFireIndex].stablePressed;
}

String formatJoystickState(const JoystickState& state) {
  String value;
  value.reserve(48);
  value += "UP=";
  value += state.up ? "1" : "0";
  value += " DOWN=";
  value += state.down ? "1" : "0";
  value += " LEFT=";
  value += state.left ? "1" : "0";
  value += " RIGHT=";
  value += state.right ? "1" : "0";
  value += " FIRE=";
  value += state.fire ? "1" : "0";
  return value;
}
