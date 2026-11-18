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


## 4. Control the Arduino from your phone

Your Arduino Mega must remain plugged into your Windows PC by USB. The PC runs a small private bridge, and Cloudflare Tunnel securely forwards the phone's connection to that bridge. The phone and PC do not need to be on the same Wi-Fi network.

### One-time Windows setup

1. Install Python 3 from https://www.python.org/downloads/windows/ and enable **Add Python to PATH** during setup.
2. Download and install **cloudflared** for Windows from https://developers.cloudflare.com/cloudflare-one/connections/connect-networks/downloads/.
3. Open Command Prompt in the downloaded repository folder and run:
   `py -m pip install -r bridge/requirements.txt`
4. Find the Arduino COM port in Windows Device Manager under **Ports (COM & LPT)**. Close Arduino IDE Serial Monitor first.

### Start the bridge

Open one Command Prompt window in the repository folder and run (replace COM5 with your actual port and create your own long private token):

```bat
py bridge/phone_bridge.py --port COM5 --token "PUT-YOUR-OWN-LONG-RANDOM-TOKEN-HERE"
```

Leave this window open. It must stay running while you use the phone.

Open a **second** Command Prompt window and run:

```bat
cloudflared tunnel --url http://localhost:8765
```

Cloudflared prints a temporary address ending in `.trycloudflare.com`. Keep this window open too. The bridge address for the website is that same address with `https://` changed to `wss://`, for example `wss://example.trycloudflare.com`.

### Connect your phone

1. Open https://arduino-servo-control.pages.dev on your phone.
2. In the Connection section, paste the `wss://` address from cloudflared.
3. Enter the exact private token you used in the first Command Prompt window.
4. Tap **Connect phone to PC**. Once connected, use the angle, speed, sweep, and stop controls normally.

The temporary tunnel address changes when you restart cloudflared. Use the new address next time. Do not share your pairing token or tunnel address. Anyone with both could send commands to your servo. Stop access by closing the cloudflared and bridge windows. The bridge only accepts the documented servo commands and validates angle/speed ranges.

**Important:** Keep the servo powered safely. A separate regulated 5V supply is usually recommended, with its ground connected to Mega GND. Start with small movements and supervise the mechanism.
