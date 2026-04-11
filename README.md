# Wico Retro Joystick

Ett PlatformIO-projekt för att bygga om en klassisk Wico Command Control-joystick till en trådlös BLE-joystick med ESP32-C3 SuperMini.

Version 1 fokuserar på att läsa joystickens microswitchar via GPIO, exponera enheten som BLE gamepad och göra första tester via USB-ström.

## Projektstruktur

- `platformio.ini`
- `include/config.h`
- `include/pins.h`
- `include/joystick.h`
- `src/main.cpp`
- `src/joystick.cpp`
- `README.md`

## Hårdvara

- ESP32-C3 SuperMini
- Wico Command Control-joystick med fem signaler:
  - upp
  - ner
  - vänster
  - höger
  - fire
- USB för första tester och strömförsörjning

Planerade senare tillägg:

- LiPo 3.7 V
- laddmodul
- on/off slide switch
- sleep mode
- batterimätning via ADC och spänningsdelare

## Kopplingsprincip

Varje microswitch kopplas mellan en GPIO och gemensam GND.

- ena sidan av varje switch till respektive GPIO
- andra sidan till gemensam GND
- intern `INPUT_PULLUP` används i koden

Det betyder att en knapp eller riktning läses som `LOW` när switchen är aktiv.

## Pinout

Den önskade ursprungliga mappningen var `Up = GPIO2`, men version 1 ändrar detta till `GPIO7`.

Orsak:

- `GPIO2` är en strapping-pin på ESP32-C3
- om spaken hålls i läget `Up` under reset eller uppstart kan boot påverkas
- `GPIO7` är ett säkrare val för en switch mot GND i denna första version

| Funktion | Version 1 pin |
| --- | --- |
| Up | GPIO7 |
| Down | GPIO3 |
| Left | GPIO4 |
| Right | GPIO5 |
| Fire | GPIO6 |

## BLE-lösning

Projektet använder biblioteket `ESP32 BLE Gamepad` tillsammans med `NimBLE-Arduino`.

I version 1 är konfigurationen medvetet enkel:

- X- och Y-axel används för riktningar
- fire mappas till `Button 1`
- inga hatt-switchar används
- auto-report är avstängt
- rapport skickas bara när joystickens tillstånd ändras

## Funktioner i version 1

- initierar alla joystick-GPIO som `INPUT_PULLUP`
- läser microswitchar med debounce
- skriver debug-info till serial monitor
- exponerar enheten som BLE gamepad
- mappar vänster/höger till X
- mappar upp/ner till Y
- mappar fire till knapp 1
- skickar bara BLE-uppdatering när state faktiskt ändras

## Bygga i PlatformIO

1. Installera PlatformIO i VS Code.
2. Öppna projektmappen.
3. Bygg projektet med PlatformIO Build.
4. Ladda upp firmware till ESP32-C3 via USB.
5. Öppna serial monitor på `115200` baud.

Projektet är konfigurerat för:

- board: `esp32-c3-devkitm-1`
- framework: Arduino

Detta är avsiktligt valt som en stabil generisk ESP32-C3-target i PlatformIO även om hårdvaran är en SuperMini-variant.

## Testa via USB innan batteri används

Första verifiering sker helt utan batteri:

1. Anslut ESP32-C3 SuperMini via USB.
2. Ladda upp firmware.
3. Öppna serial monitor.
4. Kontrollera att ändringar i riktning och fire skrivs ut i loggen.
5. Para enheten som Bluetooth gamepad från värdsystemet.
6. Testa att X/Y och knapp 1 reagerar i ett gamepad-testverktyg eller direkt på Raspberry Pi 5 / PiMiga.

## Kompatibilitetsnoteringar

Det finns några viktiga saker att känna till för ESP32-C3 och BLE gamepad:

1. `ESP32 BLE Gamepad` använder numera NimBLE och innehåller en känd anpassning för ESP32-C3, vilket gör biblioteket rimligt att använda här.
2. HID-deskriptorn kan cacheas av värdsystemet. Om du ändrar antal knappar eller axlar senare kan du behöva ta bort parkopplingen och para om enheten.
3. BLE HID fungerar normalt bra på Linux och Raspberry Pi, men exakt mapping kan variera mellan testverktyg och operativsystem.
4. GPIO2 bör undvikas för en sådan här switchingång på ESP32-C3 när signalen går till GND, eftersom det är en strapping-pin.

Om biblioteket senare visar sig ge problem i just din PiMiga-miljö är nästa rimliga fallback att gå över till en mer egen NimBLE HID-implementation, men det behövs inte i version 1.

## Nästa steg

Planerade moduler och funktioner för senare versioner:

- `power_manager`
- `battery_monitor`
- `sleep_manager`
- LiPo-drift
- laddkort
- fysisk strömbrytare
- sleep mode
- batteriindikering

Version 1 innehåller ingen aktiv batterikod eller sleep-kod, men projektstrukturen och konfigurationen är hållna enkla så att detta kan läggas till senare utan större omtag.