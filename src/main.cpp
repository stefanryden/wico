#include <Arduino.h>

#include "ble_gamepad.h"
#include "config.h"
#include "gpio_diagnostic.h"

namespace {

void logLine(const String& value) {
  if (Config::kEnableSerialDebug) {
    Serial.println(value);
  }
}

}  // namespace

void setup() {
  Serial.begin(Config::kSerialBaud);
  delay(250);

  logLine("Wico joystick firmware startup.");

  if (Config::kAppMode == Config::AppMode::kDiagnostic) {
    logLine("Active mode: GPIO diagnostic");
    GpioDiagnostic::begin();
  } else {
    logLine("Active mode: BLE gamepad");
    BleGamepadApp::begin();
  }
}

void loop() {
  if (Config::kAppMode == Config::AppMode::kDiagnostic) {
    GpioDiagnostic::loop();
  } else {
    BleGamepadApp::loop();
  }

  delay(Config::kPollIntervalMs);
}
