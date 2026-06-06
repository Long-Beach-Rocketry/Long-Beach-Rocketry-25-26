# TODO — LBR nanopb + USART Pipeline

---

## Compilation Blockers (Fix First)

### 1. Undefined variable `final_msg`
- **File**: `common/core/network/pipeline/usart_pipe.cc:42`
- The line computing it is commented out with invalid syntax but is still referenced:
  ```cpp
  // int final_msg = (frame & length) + payload_len + crc_len;  // commented out, invalid
  usart.send(..., final_msg);  // final_msg doesn't exist -> compile error
  ```
- **Fix**: `size_t final_msg = kHeaderLen + payload_len + kCrcLen;`

### 2. Class name typo — `Ringbuffer` vs `RingBuffer`
- **File**: `common/core/network/pipeline/usart_pipe.h:63`
- Uses `Ringbuffer<uint8_t, 256>` (lowercase 'b')
- Actual class in `common/core/container/ring_buffer.h` is `RingBuffer` (capital B)
- **Fix**: Rename to `RingBuffer<uint8_t, 256>`

---

## Missing: CRC Implementation

CRC is fully stubbed — 4 bytes (`kCrcLen`) are reserved in the frame but never used.

- **Send side** (`usart_pipe.cc:33-36`): Comment says "// Add CRC" — nothing is computed or written
- **Receive side** (`usart_pipe.cc:130-135`): Comment says "// Calculate CRC" — nothing is extracted or validated
- No CRC function or library exists anywhere in the codebase

**What needs to be done:**
- [ ] Add a CRC function (XMODEM-CRC16 or CRC-32 recommended)
- [ ] In `send()`: compute CRC over `[frame_id + length + payload]`, append to buffer before transmit
- [ ] In `receive()`: extract received CRC, recompute over received data, compare — drop frame on mismatch

---

## Missing: RS485 Transceiver Control (DE/RE Pins)

RS485 is half-duplex and requires GPIO control of DE (Driver Enable) and RE (Receiver Enable) pins on the transceiver chip to switch between TX and RX mode. None of this exists.

- **`usart_pipe.h:17`**: `TODO: Need to add RS485 transceiver module`
- **`usart_pipe.cc:44`**: `TODO: Add RS485 transmission logic here`
- **`usart_pipe.cc:61`**: `TODO: Add RS485 reception logic here`
- **`app/usart_pipe_test/board.h:21`**: `TODO: Add RS485 control pin if needed`
- **`app/usart_pipe_test/BSP_H723/h723_board.cc:7`**: `TODO: Need to add RS485`

**What needs to be done:**
- [ ] Add a GPIO pin (or two: DE + RE, often tied together) for transceiver direction control
- [ ] Assert DE high (enable transmit) before calling `usart.send()`
- [ ] De-assert DE (enable receive) after send completes
- [ ] Wire the GPIO into the `Pipeline` class constructor or board config

---

## Bug: Encoder Writes to Wrong Buffer Offset

- **File**: `common/core/network/pipeline/usart_pipe.cc:23`
- `payload_start` is computed correctly but then ignored:
  ```cpp
  uint8_t* payload_start = tx_buffer.data() + kHeaderLen;  // correct pointer, never used
  int payload_len = msg->encode(tx_buffer.data(), tx_buffer.size());  // writes from offset 0 -> overwrites header
  ```
- The encoder stomps on the frame ID and length bytes.
- **Fix**: `int payload_len = msg->encode(payload_start, payload_capacity);`

---

## Bug: `receive()` Never Decodes the Message

- **File**: `common/core/network/pipeline/usart_pipe.cc`
- Receive flow:
  1. Polls USART for data ✓
  2. Searches for frame ID ✓
  3. Validates header length ✓
  4. Returns `true` ← without calling `process_frame()`, extracting CRC, or decoding the protobuf
- All received frames are silently accepted but the payload is never decoded.

**What needs to be done:**
- [ ] After finding a valid frame, copy payload bytes into a temp buffer
- [ ] Validate CRC (see CRC section above)
- [ ] Call `process_frame()` / `msg->decode()` on the payload
- [ ] Surface the decoded message to the caller

---

## I2C Driver (STM32L4) — Infinite Busy-Wait Loops

- **File**: `common/drivers/platform/stm32l4/st_i2c.cc`
- 20+ polling loops with no timeout, e.g. at lines 78, 85, 100, 109, 153, 158, 164, 179, 188, 234, 244, 259, 305, 310, 320, 335, 382, 391, 439, 454:
  ```cpp
  while (!(_base_addr->ISR & I2C_ISR_TXIS))
  {
      // TODO: replace while with timeout
  }
  ```
- If an I2C device doesn't respond, the CPU hangs forever.
- The STM32H7 driver (`common/drivers/platform/stm32h7/st_i2c.cc`) already has a `timed_out()` helper — port it to L4.

**What needs to be done:**
- [ ] Port `timed_out()` from H7 driver to L4 driver
- [ ] Wrap every busy-wait with a timeout check and return an error code on expiry

---

## SPI Driver (STM32L4) — Missing Timeouts

- **File**: `common/drivers/platform/stm32l4/st_spi.cc` (lines 32, 43)
- Same pattern as I2C — infinite polling loops marked `// TODO: ADD a timeout`

**What needs to be done:**
- [ ] Add timeout logic to SPI TX/RX polling loops

---

## Priority Summary

| # | Issue | File | Severity |
|---|-------|------|----------|
| 1 | `final_msg` undefined | `usart_pipe.cc:42` | Compile error |
| 2 | `Ringbuffer` class name typo | `usart_pipe.h:63` | Compile error |
| 3 | CRC not implemented | `usart_pipe.cc` | Data corruption, silent |
| 4 | RS485 DE/RE GPIO missing | `usart_pipe.cc/h` | Bus won't work |
| 5 | Encoder writes to wrong offset | `usart_pipe.cc:23` | Frame header corrupted |
| 6 | `receive()` never decodes | `usart_pipe.cc` | All RX frames dropped |
| 7 | I2C L4 infinite busy-waits | `st_i2c.cc (stm32l4)` | CPU hang on bad device |
| 8 | SPI L4 missing timeouts | `st_spi.cc (stm32l4)` | CPU hang on bad device |
