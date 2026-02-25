require("dotenv").config();
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const { createClient } = require("@supabase/supabase-js");

const SERIAL_PORT = process.env.SERIAL_PORT || "/dev/tty.usbmodem14303";
const SERIAL_BAUD = parseInt(process.env.SERIAL_BAUD || "115200", 10);
const SUPABASE_URL = process.env.SUPABASE_URL;
const SUPABASE_ANON_KEY = process.env.SUPABASE_ANON_KEY;

if (!SUPABASE_URL || !SUPABASE_ANON_KEY) {
  console.error("Missing SUPABASE_URL or SUPABASE_ANON_KEY in .env");
  process.exit(1);
}

const supabase = createClient(SUPABASE_URL, SUPABASE_ANON_KEY);

function connect() {
  console.log(`Opening ${SERIAL_PORT} at ${SERIAL_BAUD} baud...`);

  const port = new SerialPort({
    path: SERIAL_PORT,
    baudRate: SERIAL_BAUD,
  });

  const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));

  port.on("open", () => {
    console.log("Serial port open. Waiting for readings...");
  });

  parser.on("data", async (line) => {
    console.log(`Received: ${line}`);

    let data;
    try {
      data = JSON.parse(line);
    } catch {
      console.warn("Ignoring non-JSON line");
      return;
    }

    if (typeof data.temp_c !== "number") {
      console.warn("Missing temp_c field, skipping");
      return;
    }

    const { error } = await supabase
      .from("temperature_readings")
      .insert({ temp_c: data.temp_c });

    if (error) {
      console.error("Supabase insert error:", error.message);
    } else {
      console.log(`Inserted: ${data.temp_c}°C`);
    }
  });

  port.on("error", (err) => {
    console.error("Serial error:", err.message);
  });

  port.on("close", () => {
    console.log("Serial port closed. Reconnecting in 3s...");
    setTimeout(connect, 3000);
  });
}

connect();
