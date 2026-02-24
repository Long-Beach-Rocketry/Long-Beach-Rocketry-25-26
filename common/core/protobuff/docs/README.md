# Protobuf Integration Guide

## Overview
This document explains how to use the modular protobuf (nanopb) integration in this project, both for simple encode/decode and for communication over USART. The architecture is designed for flexibility and portability between embedded and host environments.

---

## 1. Core Abstractions

### `CmdMessage` (Generic Interface)
- **Location:** `common/core/protobuff/cmd_msg.h`
- **Purpose:** Abstract base class for command messages. Defines the generic interface for encoding and decoding messages, regardless of the underlying protocol.
- **Usage:** Inherit from this class to implement protocol-specific command messages.

---

## 2. Protobuf Implementation

### `PbCmd` (Protobuf Command Implementation)
- **Location:** `common/core/protobuff/pb_cmd.h` / `pb_cmd.cc`
- **Purpose:** Implements `CmdMessage` for messages defined by nanopb/protobuf. Wraps a `RocketStructProto` (or other generated message) and provides encode/decode methods.
- **Usage Example:**

```cpp
#include "pb_cmd.h"

LBR::PbCmd cmd;
cmd.msg.Year = 2026;
// ... set other fields ...

// Encode
uint8_t buffer[128];
int len = cmd.encode(buffer, sizeof(buffer));

// Decode
LBR::PbCmd rx_cmd;
rx_cmd.decode(buffer, len);
```

---

## 3. USART Integration

### Helper Functions (`usart_pb_helper`)
- **Location:** `app/usart_app/pb_helper/usart_pb_helper.h` / `.cc`
- **Purpose:** Provides free functions to send and receive any `CmdMessage` (including `PbCmd`) over a `Usart` interface.
- **Key Functions:**
	- `send_cmd_msg(const CmdMessage*, Usart&)`
	- `receive_cmd_msg(CmdMessage*, Usart&, size_t maxlen = 128)`
- **Usage Example:**

```cpp
#include "pb_cmd.h"
#include "usart_pb_helper.h"

LBR::PbCmd tx_cmd;
tx_cmd.msg.Year = 2026;
// ... set other fields ...

send_cmd_msg(&tx_cmd, board.usart);

LBR::PbCmd rx_cmd;
receive_cmd_msg(&rx_cmd, board.usart);
```

---

## 4. Summary of Structure
- `cmd_msg.h`: Generic interface for command messages (protocol-agnostic).
- `pb_cmd.h`/`.cc`: Protobuf-specific implementation of `CmdMessage`.
- `usart_pb_helper.h`/`.cc`: Helper functions for sending/receiving `CmdMessage` over USART (used in `usart_app`).

This modular approach allows you to:
- Use protobuf encode/decode in any context (host or embedded).
- Easily swap or extend message protocols by implementing new `CmdMessage` subclasses.
- Cleanly separate protocol logic from transport (USART, etc.) and application logic.
will write about how to use protobuff