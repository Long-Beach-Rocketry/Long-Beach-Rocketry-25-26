## Structuring sensor data around `RocketMessage`

This documents the convention for wiring new sensors into `protobuff.proto`
and getting their data onto the wire, for the airbrake app and any future
board that sends telemetry over the `Pipeline`.

### 1. `RocketMessage` is the sensor-data struct | don't make another one

`protobuff.proto` defines `RocketMessage` as a container of sub-messages
(`StatusData`, `IMUData`, `BaroData`, ...). nanopb generates it as a plain C
struct in `built/protobuff.pb.h`, and `PbCmd` (`pb_cmd.h`) just wraps that
struct with `encode()`/`decode()`.

```cpp
class PbCmd
{
public:
    RocketMessage msg;   // <- this already is your "sensor data struct"
    int encode(uint8_t* buffer, size_t maxlen) const;
    bool decode(const uint8_t* buffer, size_t len);
};
```

Don't add a parallel `SensorData` struct that mirrors `RocketMessage` and
then gets copied over field-by-field before encoding. That's a second
source of truth that will drift the first time someone adds a field to one
and forgets the other. Read sensors straight into `PbCmd.msg`.

### 2. Adding a new sensor to the schema

Follow the existing pattern in `protobuff.proto`:

```proto
message NewSensorData {
    float some_value = 1;  // units in a comment, e.g. Pascals, m/s^2, rad/s
}

message RocketMessage {
    StatusData     status       = 1;
    IMUData        imu          = 2;
    BaroData       baro         = 3;
    NewSensorData  new_sensor   = 4;
    uint32         timestamp_us = 5;
}
```

Each top-level sub-message gets a matching `has_x` presence flag on
`RocketMessage` (nanopb generates these automatically for proto3 message
fields — see `has_status` / `has_imu` / `has_baro` in
`built/protobuff.pb.h`). Set that flag when you populate the sub-message so
a receiver can tell whether that sensor was actually sampled this frame.

Regenerate with the existing `msg.ps1` script per
[docs/README.md](README.md) after editing the `.proto` — never hand-edit
`built/protobuff.pb.h` / `.pb.c`.

For the airbrake variant specifically (servo position, deploy-angle
feedback, battery voltage, etc.), add each as its own sub-message the same
way `IMUData`/`BaroData` were added, rather than flat fields bolted onto
`RocketMessage` directly. Keeping one sub-message per physical sensor keeps
the `Board` struct and the sampling function below in 1:1 correspondence
with the schema, so a missing sensor is obvious at a glance in all three
places.

### 3. One function to sample, the existing function to send

Keep acquisition and transport as two separate steps:

- **Sampling** (new) — reads every driver on `Board` and fills `PbCmd.msg`.
  This is the only thing that needs to be written; it's currently done
  inline with hardcoded literals in `app/usart_pipe_test/main.cc`.
- **Transport** (already exists) — `Pipeline::send(&tx, board.usart)`
  ([usart_pipe.h:33](../network/pipeline/usart_pipe.h#L33)) already handles
  nanopb encode + framing + CRC32 + USART TX. Nothing here needs to change
  or be duplicated.

```cpp
void sample_sensors(Board& board, RocketMessage& msg)
{
    Bno055Data imu_data{};
    msg.has_imu = board.imu.read_all(imu_data);
    if (msg.has_imu)
    {
        msg.imu.accel_x = imu_data.accel.x;
        msg.imu.accel_y = imu_data.accel.y;
        msg.imu.accel_z = imu_data.accel.z;
        msg.imu.gyro_x  = imu_data.gyro.x;
        // ... gyro_y/z, quat_*, linear_accel_*, gravity_*
    }

    msg.has_baro = true;
    msg.baro.air_pressure = board.baro.get_pressure();
    msg.baro.temperature  = board.baro.get_temperature();
    // msg.baro.altitude: no driver computes this yet — derive from
    // pressure (barometric formula) or leave until that's added.

    // msg.has_status / msg.status.*: no flight-state machine exists yet
    // (see FswStatus/RocketState/AirbrakeStatus TODO in protobuff.proto).
    // Leave unset, or wire up once that logic lands.

    msg.timestamp_us = get_us();
}
```

Main loop becomes one call to gather, one call to ship:

```cpp
sample_sensors(board, tx.msg);
board.pipeline.send(&tx, board.usart);
```

Adding a sensor later means: add its sub-message to the `.proto`,
regenerate, add the matching field(s) to `Board`, add one block to
`sample_sensors()`. `Pipeline::send()` never has to change.
