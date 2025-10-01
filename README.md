
# SpotWelding+ (Web UI Only, AP-only)

**Goal:** Build #1 success on first try. Web UI modular (HTML/CSS/JS), AP-only, Web Audio beep (no physical buzzer), configurable sensors (ACS712/ZMPT), ready CI.

## Quick Start (HP-friendly)
1. Create a new GitHub repo and upload this ZIP (or push via Git).
2. Go to **Actions** → run **Build Firmware** workflow.
3. Download `firmware.bin` artifact.
4. Power ESP32, connect HP to AP `SPOTWELD+` (pass: `weld12345`).
5. Open `http://192.168.4.1/` → `OTA` tab → upload the `firmware.bin` (first build).

> This first build is a safe **skeleton** (SIM_MODE=1) that compiles fast. Next update will enable full FSM weld control.

## Defaults
- AP-only SSID: `SPOTWELD+` / pass: `weld12345`
- Pins: SSR=GPIO26, ACS712=GPIO34, ZMPT=GPIO35
- Sensor default: ACS712-30A (change in Settings → Sensors)

## Notes
- Toolchain pinned: `espressif32@6.6.0`, Arduino core pinned.
- No external libs/CDNs. Static assets served from firmware.
- Logs and error ledger will be added in subsequent commits.
