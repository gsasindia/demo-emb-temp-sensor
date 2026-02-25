"use client";

import { useEffect, useState } from "react";
import { getSupabase } from "@/lib/supabase";
import { TempChart } from "./temp-chart";

interface Reading {
  id: number;
  temp_c: number;
  recorded_at: string;
}

export function TempDashboard() {
  const [readings, setReadings] = useState<Reading[]>([]);
  const [loading, setLoading] = useState(true);
  const [connected, setConnected] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    let supabase;
    try {
      supabase = getSupabase();
    } catch (e) {
      setError(
        "Supabase not configured. Add NEXT_PUBLIC_SUPABASE_URL and NEXT_PUBLIC_SUPABASE_ANON_KEY to .env.local"
      );
      setLoading(false);
      return;
    }

    async function fetchReadings() {
      const sb = getSupabase();
      const { data, error: fetchErr } = await sb
        .from("temperature_readings")
        .select("*")
        .order("recorded_at", { ascending: true })
        .limit(50);

      if (fetchErr) {
        console.error("Fetch error:", fetchErr.message);
      } else {
        setReadings(data ?? []);
      }
      setLoading(false);
    }

    fetchReadings();

    const channel = supabase
      .channel("temperature_readings")
      .on(
        "postgres_changes",
        {
          event: "INSERT",
          schema: "public",
          table: "temperature_readings",
        },
        (payload) => {
          const newReading = payload.new as Reading;
          setReadings((prev) => {
            const updated = [...prev, newReading];
            return updated.slice(-50);
          });
        }
      )
      .subscribe((status) => {
        setConnected(status === "SUBSCRIBED");
      });

    return () => {
      getSupabase().removeChannel(channel);
    };
  }, []);

  const latest = readings.length > 0 ? readings[readings.length - 1] : null;

  if (error) {
    return (
      <div
        className="rounded-lg bg-m-white p-6"
        style={{
          border: "1px solid var(--m-border)",
          boxShadow: "var(--m-shadow-sm)",
        }}
      >
        <p className="text-sm text-m-danger">{error}</p>
      </div>
    );
  }

  if (loading) {
    return (
      <div className="space-y-4">
        <div className="h-32 animate-pulse rounded-lg bg-m-border" />
        <div className="h-80 animate-pulse rounded-lg bg-m-border" />
      </div>
    );
  }

  return (
    <div className="space-y-4">
      {/* Status indicator */}
      <div className="flex items-center gap-2 text-sm">
        <span
          className="inline-block h-2 w-2 rounded-full"
          style={{
            backgroundColor: connected
              ? "var(--m-signal)"
              : "var(--m-danger)",
          }}
        />
        <span className="text-m-ink/60">
          {connected ? "Live" : "Disconnected"}
        </span>
      </div>

      {/* Current temperature card */}
      <div
        className="rounded-lg bg-m-white p-6"
        style={{
          border: "1px solid var(--m-border)",
          boxShadow: "var(--m-shadow-sm)",
        }}
      >
        <p className="mb-1 text-sm font-medium text-m-ink/60">
          Current Temperature
        </p>
        {latest ? (
          <p className="font-mono text-5xl font-semibold text-m-primary">
            {latest.temp_c.toFixed(1)}°C
          </p>
        ) : (
          <p className="font-mono text-5xl font-semibold text-m-ink/20">
            --.-°C
          </p>
        )}
        {latest && (
          <p className="mt-2 font-mono text-xs text-m-ink/40">
            {new Date(latest.recorded_at).toLocaleString()}
          </p>
        )}
      </div>

      {/* Chart card */}
      <div
        className="rounded-lg bg-m-white p-6"
        style={{
          border: "1px solid var(--m-border)",
          boxShadow: "var(--m-shadow-sm)",
        }}
      >
        <p className="mb-4 text-sm font-medium text-m-ink/60">
          Last 50 Readings
        </p>
        <TempChart readings={readings} />
      </div>
    </div>
  );
}
