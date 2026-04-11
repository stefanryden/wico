#ifndef WICO_CONFIG_H
#define WICO_CONFIG_H

#include <Arduino.h>

namespace Config {

static constexpr uint32_t kSerialBaud = 115200;
static constexpr uint32_t kPollIntervalMs = 5;
static constexpr uint32_t kDebounceMs = 20;

inline constexpr char kBleDeviceName[] = "Wico Retro Joystick";
inline constexpr char kBleManufacturer[] = "stefanryden";
static constexpr uint8_t kBleInitialBatteryLevel = 100;

static constexpr int16_t kAxisMin = 0;
static constexpr int16_t kAxisMax = 32767;
static constexpr int16_t kAxisCenter = (kAxisMin + kAxisMax) / 2;

static constexpr bool kEnableSerialDebug = true;

// Future expansion placeholders for battery-powered revisions.
static constexpr bool kEnableBatterySupport = false;
static constexpr uint8_t kBatteryAdcPin = 255;
static constexpr uint8_t kChargeStatusPin = 255;
static constexpr uint8_t kPowerSwitchPin = 255;

}  // namespace Config

#endif
