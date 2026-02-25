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

### 2. Firmware (Keil Studio)

#### Prerequisites
- A [Keil Studio Cloud](https://studio.keil.arm.com) account (free with Arm login)
- NUCLEO-U575ZI-Q connected via USB
- Chrome or Edge browser (needed for WebUSB flashing)

#### Steps

1. Open [Keil Studio Cloud](https://studio.keil.arm.com) in your browser
2. Click **File → Import Project…** and paste this repo URL:
   ```
   https://github.com/gsasindia/demo-emb-temp-sensor
   ```
3. Keil Studio will detect the `firmware/` directory. If prompted, set:
   - **Target**: `NUCLEO-U575ZI-Q`
   - **Toolchain**: AC6 (Arm Compiler 6) or GCC
4. Ensure the following source files are included in the build:
   - `firmware/src/main.c`
   - `firmware/src/temp_sensor.c`
5. You may need to add the STM32U5 CMSIS/HAL pack if not auto-resolved:
   - Open **CMSIS Pack Manager** (sidebar)
   - Search for `STM32U5xx_DFP` and install it
6. Click **Build** (hammer icon) — confirm it compiles with no errors
7. Click **Run** (play icon) to flash the firmware to the board
8. Open the built-in **Serial Monitor** (or any serial terminal):
   - Port: the ST-Link VCP (auto-detected)
   - Baud: **115200**
9. You should see a JSON line every 2 minutes:
   ```json
   {"temp_c":24.7,"ts":120000}
   ```
   The green LED (LD1) blinks on each reading.

#### Troubleshooting
- **Board not detected**: Make sure you're using Chrome/Edge and click "Connect Board" in the bottom status bar
- **No serial output**: Verify LPUART1 is routed to ST-Link VCP (PA9/PA10 — default on this Nucleo)
- **Build errors about missing HAL headers**: Install the `STM32U5xx_DFP` pack from the CMSIS Pack Manager

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
