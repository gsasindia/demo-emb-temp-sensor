# CLAUDE.md — demo-emb-temp-sensor

## Project Overview
Temperature monitoring station using the Nucleo-U575ZI-Q board. The board reads its internal temp sensor every 2 minutes and sends JSON readings over USB serial (ST-Link VCP). A Node.js bridge script pushes data to Supabase. A Next.js dashboard displays live data.

## Architecture
```
[Nucleo-U575ZI-Q] --> [Serial Bridge (Node.js)] --> [Supabase Postgres] --> [Next.js Dashboard]
```

## Repository Structure
- `firmware/` — STM32 HAL firmware (C, snake_case)
- `serial-bridge/` — Node.js serial-to-Supabase bridge
- `supabase/` — Database schema
- `dashboard/` — Next.js App Router + Tailwind + Recharts

## Conventions
- **Commits**: Conventional Commits — `type(scope): description`
- **C files**: `snake_case.c` / `snake_case.h`
- **Branches**: `feat/`, `fix/`, `chore/`
- **Design**: Meridian design language (see dashboard/tailwind.config.ts for tokens)

## Key Commands
- **Serial Bridge**: `cd serial-bridge && npm install && node bridge.js`
- **Dashboard**: `cd dashboard && npm install && npm run dev`
- **Firmware**: Build via Keil Studio or STM32CubeIDE, flash to Nucleo-U575ZI-Q

## Environment Variables
- Serial Bridge: see `serial-bridge/.env.example`
- Dashboard: see `dashboard/.env.local.example`
