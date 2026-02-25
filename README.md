# demo-emb-temp-sensor

Temperature monitoring station using the Nucleo-U575ZI-Q internal temp sensor, streaming live data to a web dashboard.

## Hardware Required

| Item | Notes |
|------|-------|
| NUCLEO-U575ZI-Q | STM32U575 dev board |
| USB cable (Type-C or Micro-B) | ST-Link VCP connection to host PC |

No external sensors or wiring needed — uses the STM32U575's internal temperature sensor.

## Architecture

```
[Nucleo-U575ZI-Q]              [Host PC]                    [Cloud]
 Internal Temp Sensor  --->  Serial Bridge (Node.js)  --->  Supabase (Postgres)
 LPUART1 / VCP / USB         reads serial, inserts rows          ^
                                                                  |
                                                        Next.js Dashboard
                                                        Meridian design language
```

## Setup

### 1. Supabase

1. Create a free project at [supabase.com](https://supabase.com)
2. Open the SQL Editor and run `supabase/schema.sql`
3. Copy your project URL and anon key

### 2. Firmware (Keil Studio for VS Code)

#### Prerequisites
- [VS Code](https://code.visualstudio.com/) installed
- **Arm Keil Studio Pack** extension installed from the VS Code Marketplace (extension ID: `Arm.keil-studio-pack`)
- **CMSIS-Toolbox** and **Arm Compiler 6** — installed automatically by the extension on first launch
- NUCLEO-U575ZI-Q connected via USB

#### Steps

1. Clone this repo and open it in VS Code:
   ```bash
   git clone https://github.com/gsasindia/demo-emb-temp-sensor.git
   code demo-emb-temp-sensor
   ```
2. Open the **CMSIS** sidebar (Arm icon in the Activity Bar)
3. Click **Create New Solution** (or open an existing `.csolution.yml` if present):
   - **Device**: `STM32U575ZITxQ`
   - **Compiler**: AC6 (Arm Compiler 6)
4. Add the source files to your project:
   - `firmware/src/main.c`
   - `firmware/src/temp_sensor.c`
   - `firmware/src/main.h`
   - `firmware/src/temp_sensor.h`
5. Add required CMSIS packs via the **Manage Software Packs** view:
   - `Keil::STM32U5xx_DFP` (device family pack)
   - `ARM::CMSIS` (core CMSIS headers)
6. Click **Build** in the CMSIS sidebar — confirm it compiles with no errors
7. Click **Run** (or **Download to Device**) to flash the firmware to the board
8. Open a serial terminal to verify output:
   - In VS Code: use the **Serial Monitor** extension or the built-in terminal with a tool like `screen`:
     ```bash
     # macOS
     screen /dev/tty.usbmodem* 115200
     # Linux
     screen /dev/ttyACM0 115200
     # Windows — use PuTTY or the Serial Monitor extension with the appropriate COM port
     ```
   - You should see a JSON line every 2 minutes:
     ```json
     {"temp_c":24.7,"ts":120000}
     ```
   - The green LED (LD1) blinks on each reading

#### Troubleshooting
- **Board not detected**: Check USB connection; install [ST-Link drivers](https://www.st.com/en/development-tools/stsw-link009.html) if on Windows
- **No serial output**: Verify LPUART1 is routed to ST-Link VCP (PA9/PA10 — default on this Nucleo)
- **Build errors about missing HAL headers**: Ensure `Keil::STM32U5xx_DFP` is installed in the CMSIS Pack Manager
- **"No device connected"**: Click the device selector in the CMSIS sidebar and pick `NUCLEO-U575ZI-Q`

### 3. Serial Bridge

```bash
cd serial-bridge
npm install
cp .env.example .env
# Edit .env with your serial port and Supabase credentials
node bridge.js
```

### 4. Dashboard

```bash
cd dashboard
npm install
cp .env.local.example .env.local
# Edit .env.local with your Supabase credentials
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) to view the dashboard.

## Demo Script

1. Flash firmware to the board and connect USB
2. Start the serial bridge — confirm readings appear in terminal
3. Open the dashboard — observe the current temperature card and chart
4. Touch the MCU to warm it — watch the temperature rise in real time
5. Release — watch it cool back down

## Serial Output Format

```json
{"temp_c":24.7,"ts":120000}
```

- `temp_c` — Temperature in degrees Celsius (1 decimal)
- `ts` — Uptime in milliseconds
