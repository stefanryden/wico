#include <Arduino.h>
#include <BleGamepad.h>

#include "config.h"
#include "joystick.h"
#include "pins.h"

namespace {

BleGamepad bleGamepad(Config::kBleDeviceName, Config::kBleManufacturer,
                      Config::kBleInitialBatteryLevel);
JoystickInput joystick;

bool previousConnectionState = false;

void logLine(const String& value) {
  if (Config::kEnableSerialDebug) {
    Serial.println(value);
  }
}

void logPinout() {
  logLine("Pin mapping:");
  logLine("  Up    -> GPIO " + String(Pins::kUp));
  logLine("  Down  -> GPIO " + String(Pins::kDown));
  logLine("  Left  -> GPIO " + String(Pins::kLeft));
  logLine("  Right -> GPIO " + String(Pins::kRight));
  logLine("  Fire  -> GPIO " + String(Pins::kFire));
}

BleGamepadConfiguration buildBleConfiguration() {
  BleGamepadConfiguration configuration;
  configuration.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  configuration.setAutoReport(false);
  configuration.setButtonCount(1);
  configuration.setHatSwitchCount(0);
  configuration.setIncludeStart(false);
  configuration.setIncludeSelect(false);
  configuration.setWhichAxes(true, true, false, false, false, false, false, false);
  configuration.setAxesMin(Config::kAxisMin);
  configuration.setAxesMax(Config::kAxisMax);
  return configuration;
}

void sendGamepadState(const JoystickState& state) {
  const JoystickAxes axes =
      joystick.axes(Config::kAxisMin, Config::kAxisCenter, Config::kAxisMax);

  bleGamepad.setAxes(axes.x, axes.y, 0, 0, 0, 0, 0, 0);

  if (state.fire) {
    bleGamepad.press(BUTTON_1);
  } else {
    bleGamepad.release(BUTTON_1);
  }

  bleGamepad.sendReport();

  if (Config::kEnableSerialDebug) {
    Serial.print("BLE report sent. X=");
    Serial.print(axes.x);
    Serial.print(" Y=");
    Serial.print(axes.y);
    Serial.print(" FIRE=");
    Serial.println(state.fire ? "1" : "0");
  }
}

void handleConnectionState() {
  const bool connected = bleGamepad.isConnected();

  if (connected == previousConnectionState) {
    return;
  }

  previousConnectionState = connected;

  if (connected) {
    logLine("BLE gamepad connected.");
    sendGamepadState(joystick.state());
  } else {
    logLine("BLE gamepad disconnected.");
  }
}

}  // namespace

void setup() {
  Serial.begin(Config::kSerialBaud);
  delay(250);

  logLine("Wico Retro Joystick - startup");
  logLine("Using INPUT_PULLUP on all joystick inputs.");
  logLine("GPIO2 was replaced with GPIO7 to avoid ESP32-C3 strapping issues.");
  logPinout();

  joystick.begin();
  logLine("Initial state: " + formatJoystickState(joystick.state()));

  BleGamepadConfiguration bleConfiguration = buildBleConfiguration();
  bleGamepad.begin(&bleConfiguration);
  logLine("BLE gamepad initialized. Waiting for host connection.");
}

void loop() {
  handleConnectionState();

  if (joystick.update()) {
    const JoystickState& state = joystick.state();
    logLine("State changed: " + formatJoystickState(state));

    if (bleGamepad.isConnected()) {
      sendGamepadState(state);
    } else {
      logLine("BLE not connected. State change logged only to serial.");
    }
  }

  delay(Config::kPollIntervalMs);
}
