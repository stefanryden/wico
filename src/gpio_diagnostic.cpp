#include "gpio_diagnostic.h"

#include <Arduino.h>

#include "config.h"
#include "pins.h"

namespace GpioDiagnostic {
namespace {

struct InputTrack {
  const char* label;
  uint8_t pin;
  bool lastRawPressed;
};

InputTrack inputs[] = {
    {"UP", Pins::kUp, false},
    {"DOWN", Pins::kDown, false},
    {"LEFT", Pins::kLeft, false},
    {"RIGHT", Pins::kRight, false},
    {"FIRE", Pins::kFire, false},
};

uint32_t lastPeriodicLogMs = 0;

bool isPressedRaw(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

void logLine(const String& value) {
  if (Config::kEnableSerialDebug) {
    Serial.println(value);
  }
}

void logSummary(const char* prefix) {
  if (!Config::kEnableSerialDebug) {
    return;
  }

  Serial.print(prefix);
  for (size_t index = 0; index < (sizeof(inputs) / sizeof(inputs[0])); ++index) {
    const InputTrack& input = inputs[index];

    Serial.print(' ');
    Serial.print(input.label);
    Serial.print("(GPIO");
    Serial.print(input.pin);
    Serial.print(")=");
    Serial.print(isPressedRaw(input.pin) ? "LOW(active)" : "HIGH(idle)");

    if (index + 1U < (sizeof(inputs) / sizeof(inputs[0]))) {
      Serial.print(" |");
    }
  }
  Serial.println();
}

}  // namespace

void begin() {
  for (InputTrack& input : inputs) {
    pinMode(input.pin, INPUT_PULLUP);
    input.lastRawPressed = isPressedRaw(input.pin);
  }

  logLine("GPIO diagnostic mode started.");
  logLine("All switches should be wired GPIO -> switch -> GND.");
  logSummary("Initial:");
}

void loop() {
  const uint32_t now = millis();

  for (InputTrack& input : inputs) {
    const bool rawPressed = isPressedRaw(input.pin);

    if (rawPressed != input.lastRawPressed) {
      input.lastRawPressed = rawPressed;

      if (Config::kEnableSerialDebug) {
        Serial.print("RAW ");
        Serial.print(input.label);
        Serial.print("(GPIO");
        Serial.print(input.pin);
        Serial.print(") change: ");
        Serial.println(rawPressed ? "LOW(active)" : "HIGH(idle)");
      }
    }
  }

  if ((now - lastPeriodicLogMs) >= 1000) {
    lastPeriodicLogMs = now;
    logSummary("Tick:");
  }
}

}  // namespace GpioDiagnostic
