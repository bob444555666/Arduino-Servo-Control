import argparse
import asyncio
import json
import time

import serial
import websockets

DEFAULT_SERVER = "wss://servo-room-server.heyboernathan.workers.dev"

def valid_command(command):
    if command in ("STOP", "SWEEP:START", "SWEEP:STOP"):
        return True
    try:
        key, value = command.split(":", 1)
        number = int(value)
        if key == "ANGLE":
            return 0 <= number <= 180
        if key == "SPEED":
            return 1 <= number <= 180
    except (ValueError, TypeError):
        pass
    return False

async def main():
    parser = argparse.ArgumentParser(description="Connect your USB Arduino to a Cloudflare servo room")
    parser.add_argument("--port", required=True, help="Arduino COM port, e.g. COM5")
    parser.add_argument("--server", default=DEFAULT_SERVER)
    parser.add_argument("--room", help="Room code; if omitted, you will be prompted")
    args = parser.parse_args()
    room = (args.room or input("Enter the room code shown on your phone: ")).strip().upper()
    if not 4 <= len(room) <= 24 or not all(c.isalnum() or c in "_-" for c in room):
        print("Room code must be 4–24 letters, numbers, underscores or hyphens.")
        return
    try:
        board = serial.Serial(args.port, 9600, timeout=0.1, write_timeout=1)
    except Exception as exc:
        print(f"Could not open {args.port}: {exc}")
        return
    url = args.server.rstrip("/") + "/room/" + room + "?role=device"
    print(f"Arduino ready on {args.port}. Joining room {room}...")
    print("Keep this window open while controlling the servo from your phone.")
    try:
        while True:
            try:
                async with websockets.connect(url, ping_interval=20, ping_timeout=20, max_size=2048) as ws:
                    await ws.send(json.dumps({"type": "device-status", "message": "Arduino connected to room"}))
                    print("Connected! Your phone can now control the servo.")
                    async for raw in ws:
                        try:
                            data = json.loads(raw)
                            if data.get("type") == "command":
                                command = str(data.get("command", "")).strip()
                                if valid_command(command):
                                    board.write((command + "\n").encode("ascii"))
                        except (ValueError, TypeError, serial.SerialException):
                            continue
            except (OSError, websockets.WebSocketException) as exc:
                print(f"Room connection lost ({exc}). Reconnecting in 4 seconds...")
                await asyncio.sleep(4)
    except KeyboardInterrupt:
        print("\nDisconnecting...")
    finally:
        try:
            board.write(b"STOP\n")
            board.close()
        except Exception:
            pass

if __name__ == "__main__":
    asyncio.run(main())
