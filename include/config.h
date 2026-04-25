#ifndef WICO_CONFIG_H
#define WICO_CONFIG_H

#include <Arduino.h>

namespace Config {

static constexpr uint32_t kSerialBaud = 115200;
static constexpr uint32_t kPollIntervalMs = 5;
static constexpr uint32_t kDebounceMs = 20;

inline constexpr char kBleDeviceName[] = "AmigaJoystick";
inline constexpr char kBleManufacturer[] = "Stefan";
static constexpr uint8_t kBleInitialBatteryLevel = 100;

static constexpr int16_t kAxisMin = -32767;
static constexpr int16_t kAxisMax = 32767;
static constexpr int16_t kAxisCenter = 0;

static constexpr bool kEnableSerialDebug = true;

enum class AppMode : uint8_t {
  kBle = 0,
  kDiagnostic = 1,
};

#ifndef WICO_APP_MODE
#define WICO_APP_MODE 0
#endif

#if WICO_APP_MODE == 1
inline constexpr AppMode kAppMode = AppMode::kDiagnostic;
#else
inline constexpr AppMode kAppMode = AppMode::kBle;
#endif

// Future expansion placeholders for battery-powered revisions.
static constexpr bool kEnableBatterySupport = false;
static constexpr uint8_t kBatteryAdcPin = 255;
static constexpr uint8_t kChargeStatusPin = 255;
static constexpr uint8_t kPowerSwitchPin = 255;

}  // namespace Config

#endif
