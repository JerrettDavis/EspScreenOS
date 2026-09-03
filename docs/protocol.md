# Common device protocol

EspScreenOS includes a transport-independent framed protocol for host tools and peer devices. Existing USB, Python, .NET, BLE, UART, Wi-Fi, and ESP-NOW adapters can share one message model.

## Wire framing

1. Build the binary frame.
2. Append CRC-32 over header and payload.
3. Encode with COBS.
4. Append a zero delimiter.

All multi-byte integers are little-endian.

| Field | Bytes | Description |
|---|---:|---|
| Magic | 2 | `0x5345` |
| Version | 1 | Protocol version, currently 1 |
| Type | 1 | Request, response, event, error, ping, pong |
| Request ID | 4 | Correlates request and response |
| Payload size | 2 | 0 through 1024 |
| Reserved | 2 | Must be zero |
| Payload | N | Method-specific bytes |
| CRC-32 | 4 | Standard reflected polynomial |

COBS makes zero a reliable frame delimiter and supports incremental reads over UART or USB serial. The decoder validates magic, version, exact length, payload bound, and CRC before exposing a frame.

## RPC routing

`RpcRouter` maps 16-bit method IDs to a fixed-capacity table of typed `IRpcEndpoint` instances. Composition supplies declarative `RpcRoute` batches. The router validates the complete batch—including null endpoints, reserved method zero, duplicates, and remaining capacity—before committing any route, so failed registration cannot leave a partially changed table. An endpoint receives bounded request and response spans and returns `Status`; response length is reset before every dispatch. Transport adapters own buffering, timeout, and retransmission policy.

Recommended method ranges:

- `0x0001–0x00FF`: platform information and health
- `0x0100–0x01FF`: settings and provisioning
- `0x0200–0x02FF`: application control
- `0x0300–0x03FF`: storage and assets
- `0x0400–0x04FF`: hardware diagnostics
- `0x8000–0xFFFF`: project-specific methods

## Transport requirements

- Preserve complete frame bytes and delimiter order.
- Apply backpressure; do not allocate unbounded receive buffers.
- Authenticate or physically constrain any transport that can mutate settings or files.
- Keep RPC handlers short. Queue long operations and return a job ID.
- Use the same method IDs and payload schema across transports.

The base repository supplies framing and routing. Project firmware can add CBOR, protobuf, or carefully bounded JSON payload conventions without changing the transport envelope.
