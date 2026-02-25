"use client";

import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";

interface Reading {
  id: number;
  temp_c: number;
  recorded_at: string;
}

export function TempChart({ readings }: { readings: Reading[] }) {
  const data = readings.map((r) => ({
    time: new Date(r.recorded_at).toLocaleTimeString([], {
      hour: "2-digit",
      minute: "2-digit",
    }),
    temp_c: r.temp_c,
  }));

  if (data.length === 0) {
    return (
      <div className="flex h-64 items-center justify-center text-m-ink/50">
        No readings yet
      </div>
    );
  }

  return (
    <ResponsiveContainer width="100%" height={300}>
      <LineChart data={data}>
        <CartesianGrid strokeDasharray="3 3" stroke="var(--m-border)" />
        <XAxis
          dataKey="time"
          tick={{ fontSize: 12, fill: "var(--m-ink)" }}
          tickLine={false}
        />
        <YAxis
          domain={["auto", "auto"]}
          tick={{ fontSize: 12, fill: "var(--m-ink)" }}
          tickLine={false}
          unit="°C"
        />
        <Tooltip
          contentStyle={{
            background: "var(--m-white)",
            border: "1px solid var(--m-border)",
            borderRadius: 8,
            fontFamily: "var(--font-mono), monospace",
          }}
          formatter={(value: number | undefined) => [
            `${(value ?? 0).toFixed(1)}°C`,
            "Temp",
          ]}
        />
        <Line
          type="monotone"
          dataKey="temp_c"
          stroke="var(--m-chart-1)"
          strokeWidth={2}
          dot={{ r: 3, fill: "var(--m-chart-1)" }}
          activeDot={{ r: 5, fill: "var(--m-chart-2)" }}
        />
      </LineChart>
    </ResponsiveContainer>
  );
}
