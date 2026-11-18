# Arduino Servo Control

A browser-based USB controller for an Arduino Mega and a **continuous-rotation servo** connected to digital pin 33.

## Features

- Connect/disconnect using the browser's Web Serial API
- Start clockwise, start reverse, and stop
- Live speed slider from 0% to 100%
- No server, API key, or Cloudflare Worker required

## 1. Wiring

| Servo wire | Connect to |
|---|---|
| Signal (often orange/yellow/white) | Mega digital pin 33 |
| Ground (often brown/black) | Mega GND |
| Power (often red) | Regulated 5V supply |

**Power warning:** Do not power a high-current servo from the Mega's 5V pin. Use a separate regulated 5V supply rated for the servo's stall/current requirements, and connect that supply's GND to Arduino GND (common ground). Check your servo's voltage rating before connecting it.

## 2. Upload the Arduino code

1. Open `arduino/servo_controller.ino` in the Arduino IDE.
2. Select **Arduino Mega or Mega 2560** and the correct USB port.
3. Install the built-in **Servo** library if your IDE does not already have it.
4. Upload the sketch.
5. Close the Arduino IDE Serial Monitor. The website needs exclusive access to the serial port.

## 3. Open the website

Open `index.html` in a current desktop version of Google Chrome or Microsoft Edge. Web Serial is supported on Chromium-based desktop browsers and requires a secure context (HTTPS or localhost). Click **Connect Arduino**, select the Mega's serial port, then use the direction and stop controls.

If opening the file directly is blocked by your browser, serve this folder on localhost (for example, with the VS Code Live Server extension) or deploy it to a static HTTPS host such as Cloudflare Pages.

## Servo type note

This project controls **continuous-rotation servos**. These interpret the control pulse as direction and speed. A normal positional servo interprets it as an angle and cannot spin continuously. For a positional servo, the control interface and sketch need to be changed to set an angle instead.

## Speed calibration

100% sends the configured endpoint pulse (default 2000µs clockwise and 1000µs reverse). Actual speed is limited by the servo's motor, load, supply voltage/current, and mechanics. If it creeps when stopped, adjust `STOP_US` slightly (for example, 1490 or 1510). If the direction labels are opposite, swap the CW and CCW endpoint values.

Test at low speed first, with the servo unloaded and securely mounted. Do not force the shaft or exceed the servo's rated voltage or mechanical load.