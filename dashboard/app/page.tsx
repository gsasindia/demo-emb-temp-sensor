import { TempDashboard } from "@/components/temp-dashboard";

export const dynamic = "force-dynamic";

export default function Home() {
  return (
    <main className="mx-auto max-w-3xl px-4 py-8">
      <h1 className="mb-6 text-2xl font-semibold text-m-ink">
        Temperature Monitor
      </h1>
      <TempDashboard />
    </main>
  );
}
