#include "ble_gamepad.h"

#include <Arduino.h>
#include <BleGamepad.h>
#include <NimBLEDevice.h>

#include "config.h"
#include "joystick.h"

namespace BleGamepadApp {
namespace {

BleGamepad bleGamepad(Config::kBleDeviceName, Config::kBleManufacturer,
                      Config::kBleInitialBatteryLevel);
JoystickInput joystick;

bool previousConnectionState = false;
uint32_t lastBleStatusLogMs = 0;
uint32_t lastAdvRetryMs = 0;

void logLine(const String& value) {
  if (Config::kEnableSerialDebug) {
    Serial.println(value);
  }
}

BleGamepadConfiguration buildBleConfiguration() {
  BleGamepadConfiguration configuration;
  configuration.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  configuration.setAutoReport(false);
  configuration.setButtonCount(1);
  configuration.setHatSwitchCount(0);
  configuration.setIncludeStart(false);
  configuration.setIncludeSelect(false);
  configuration.setWhichAxes(true, true, false, false, false, false, false,
                             false);
  configuration.setAxesMin(Config::kAxisMin);
  configuration.setAxesMax(Config::kAxisMax);
  return configuration;
}

bool isBleAdvertising() {
  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  return advertising != nullptr && advertising->isAdvertising();
}

void ensureAdvertising() {
  const uint32_t now = millis();
  if ((now - lastAdvRetryMs) < 2000) {
    return;
  }
  lastAdvRetryMs = now;

  if (bleGamepad.isConnected() || isBleAdvertising()) {
    return;
  }

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  if (advertising != nullptr) {
    advertising->start();
    logLine("BLE advertising was inactive, restarted advertising.");
  }
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

void logBleStatusPeriodically() {
  if (!Config::kEnableSerialDebug) {
    return;
  }

  const uint32_t now = millis();
  if ((now - lastBleStatusLogMs) < 5000) {
    return;
  }
  lastBleStatusLogMs = now;

  Serial.print("BLE status: connected=");
  Serial.print(bleGamepad.isConnected() ? "yes" : "no");
  Serial.print(" advertising=");
  Serial.println(isBleAdvertising() ? "yes" : "no");
}

}  // namespace

void begin() {
  joystick.begin();
  logLine("BLE mode started.");
  logLine("Initial state: " + formatJoystickState(joystick.state()));

  BleGamepadConfiguration bleConfiguration = buildBleConfiguration();
  bleGamepad.begin(&bleConfiguration);

  logLine("BLE gamepad initialized.");
  logLine(String("BLE advertising active: ") +
          (isBleAdvertising() ? "yes" : "no"));
}

void loop() {
  handleConnectionState();
  ensureAdvertising();
  logBleStatusPeriodically();

  if (joystick.update()) {
    const JoystickState& state = joystick.state();
    logLine("State changed: " + formatJoystickState(state));

    if (bleGamepad.isConnected()) {
      sendGamepadState(state);
    } else {
      logLine("BLE not connected. Change logged only to serial.");
    }
  }
}

}  // namespace BleGamepadApp
