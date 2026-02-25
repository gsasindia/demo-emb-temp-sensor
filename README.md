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

### 2. Firmware

1. Open `firmware/` in STM32CubeIDE or Keil Studio
2. Build and flash to the Nucleo-U575ZI-Q
3. The board outputs JSON over USB serial at 115200 baud every 2 minutes

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
