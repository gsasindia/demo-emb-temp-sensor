CREATE TABLE temperature_readings (
  id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  temp_c REAL NOT NULL,
  recorded_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

ALTER TABLE temperature_readings ENABLE ROW LEVEL SECURITY;

CREATE POLICY "Allow public read"
  ON temperature_readings
  FOR SELECT
  USING (true);

CREATE POLICY "Allow insert with anon key"
  ON temperature_readings
  FOR INSERT
  WITH CHECK (true);
