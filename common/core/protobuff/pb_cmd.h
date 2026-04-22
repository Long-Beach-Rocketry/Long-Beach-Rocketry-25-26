/**
* @file pb_cmd.h
* @author Bex Saw
* @brief Protobuf command message wrapper for encoding/decoding RocketStructProto messages.
* @version 0.1
*/

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "protobuff.pb.h"

/**
* @note This is acting as a wrapper around the generated protobuf code 
*      to provide a simple interface for encoding and decoding messages.
*
* @brief The process of the encode/decode functions from nanopb:
*       Encode: struct → encode with nanopb → get buffer + length.
*       Decode: buffer + length → decode with nanopb → get struct.
*/

namespace LBR
{
class PbCmd
{
public:
    RocketStructProto msg;

    /**
    * @brief Encode the message into a buffer. Returns the number of bytes written, or -1 on failure.
    * @param buffer The buffer to write the encoded message into.
    * @param maxlen The maximum length of the buffer.
    */
    int encode(uint8_t* buffer, size_t maxlen) const;

    /**
    * @brief Decode the message from a buffer. Returns true on success, false on failure.
    * @param buffer The buffer containing the encoded message.
    * @param len The length of the buffer.
    */
    bool decode(const uint8_t* buffer, size_t len);

    ~PbCmd();
};
}  // namespace LBR