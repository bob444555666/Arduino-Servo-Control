import argparse
import asyncio
import json
import signal
import sys

import serial
from websockets.asyncio.server import serve

async def main():
    parser = argparse.ArgumentParser(description="Secure phone-to-Arduino USB bridge")
    parser.add_argument("--port", required=True, help="Arduino COM port, e.g. COM5")
    parser.add_argument("--token", required=True, help="Private pairing token; use the same token on the website")
    parser.add_argument("--listen", default="0.0.0.0")
    parser.add_argument("--web-port", type=int, default=8765)
    args = parser.parse_args()

    try:
        board = serial.Serial(args.port, 9600, timeout=0.1, write_timeout=1)
    except Exception as exc:
        print(f"Could not open {args.port}: {exc}")
        return

    print(f"Arduino ready on {args.port}. Bridge listening on port {args.web_port}.")
    print("Keep this window open. Press Ctrl+C to stop.")

    async def handler(ws):
        try:
            raw = await asyncio.wait_for(ws.recv(), timeout=10)
            try:
                auth = json.loads(raw)
            except Exception:
                await ws.close(code=1008, reason="Pairing required")
                return
            if auth.get("type") != "auth" or auth.get("token") != args.token:
                await ws.close(code=1008, reason="Incorrect pairing token")
                return
            await ws.send(json.dumps({"type": "ready", "message": "Arduino bridge connected"}))
            async for message in ws:
                try:
                    data = json.loads(message)
                    command = str(data.get("command", "")).strip()
                    allowed = command in ("STOP", "SWEEP:START", "SWEEP:STOP") or command.startswith("ANGLE:") or command.startswith("SPEED:")
                    if not allowed:
                        continue
                    if command.startswith("ANGLE:"):
                        value = int(command[6:])
                        if not 0 <= value <= 180: continue
                    if command.startswith("SPEED:"):
                        value = int(command[6:])
                        if not 1 <= value <= 180: continue
                    board.write((command + "\\n").encode("ascii"))
                except (ValueError, TypeError, serial.SerialException):
                    continue
        except Exception:
            pass

    async with serve(handler, args.listen, args.web_port, origins=None, max_size=2048):
        await asyncio.Future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\\nBridge stopped.")
