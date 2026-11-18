# Arduino Positional Servo Control

A browser-based USB controller for a **standard positional servo** connected to Arduino Mega digital pin **33**.

## Features
- Browser USB connection through Web Serial
- Set an angle from 0° to 180°, with left/center/right presets
- Adjustable movement rate from 1°/second to 180°/second
- Automatic back-and-forth sweep
- Stop movement and hold the current position
- No server, API key, or Cloudflare Worker required for serial control

## 1. Wiring

| Servo wire (common colors) | Connect to |
|---|---|
| Signal (orange/yellow/white) | Mega digital pin 33 |
| Ground (brown/black) | Mega GND |
| Power (red) | Suitable regulated 5V supply |

**Power warning:** A servo can draw more current than the Mega's 5V pin can safely provide. For most servos, use a separate regulated 5V supply rated for its current requirements. Connect the external supply GND to Arduino GND (common ground). Check the servo's voltage rating before powering it.

## 2. Upload the Arduino sketch
1. Open `arduino/servo_controller.ino` in the Arduino IDE.
2. Select **Arduino Mega or Mega 2560** and its USB port.
3. Make sure the Arduino **Servo** library is installed.
4. Upload the sketch.
5. Close the Arduino IDE Serial Monitor before using the website.

## 3. Use the website
Open the deployed Cloudflare Pages site or `index.html` in desktop Google Chrome or Microsoft Edge. Web Serial requires a Chromium-based desktop browser and a secure context (HTTPS or localhost).

1. Click **Connect Arduino** and select the Mega's serial port.
2. Choose a target angle or use Left, Center, and Right presets.
3. Set the movement speed in degrees per second.
4. Click **Move to angle**, or use **Start auto sweep**.
5. Click **Stop movement** to stop and hold the current position.

If opening the HTML file directly is blocked, serve the folder on localhost (for example, VS Code Live Server) or use the Cloudflare Pages HTTPS deployment.

## Servo range and speed notes
This sketch is for a **standard positional servo**, not a continuous-rotation servo. It uses `Servo.write(angle)` and steps the commanded angle gradually. The speed control sets the rate of angle changes; it does not guarantee a physical speed. Actual movement depends on the servo model, supply, load, and mechanics.

Although the UI range is 0°–180°, some servos have a smaller safe mechanical range. Start near 90° and test small movements first. Reduce the range if the servo strains, buzzes, heats up, or pushes against its end stops. Never force the shaft or exceed the servo's rated voltage or load.
