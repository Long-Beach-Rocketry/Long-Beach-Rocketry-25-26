/**
* @file pb_cmd.h
* @author Bex Saw
* @brief Protobuf command message wrapper for encoding/decoding RocketStructProto messages.
* @version 0.1
*/

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "cmd_msg.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "protobuff.pb.h"

namespace LBR
{

class PbCmd : public CmdMessage
{
public:
    // The actual protobuf message
    RocketStructProto msg;

    virtual ~PbCmd();

    /**
    * @brief Encode the message into a buffer. Returns the number of bytes written, or -1 on failure.
    * @param buffer The buffer to write the encoded message into.
    * @param maxlen The maximum length of the buffer.
    */
    int encode(uint8_t* buffer, size_t maxlen) const override;

    /**
    * @brief Decode the message from a buffer. Returns true on success, false on failure.
    * @param buffer The buffer containing the encoded message.
    * @param len The length of the buffer.
    */
    bool decode(const uint8_t* buffer, size_t len) override;
};
}  // namespace LBR