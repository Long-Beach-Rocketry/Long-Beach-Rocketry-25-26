/**
* @file cmd_msg.h
* @author Bex Saw
* @brief Interface for command messages (protobuf)
* @version 0.1
*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace LBR
{
class CmdMessage
{
public:
    virtual ~CmdMessage() = default;

    /**
    * @brief Encode the message into a buffer. Returns the number of bytes written, or -1 on failure.
    * @param buffer The buffer to write the encoded message into.
    */
    virtual int encode(uint8_t* buffer, size_t maxlen) const = 0;

    /**
    * @brief Decode the message from a buffer. Returns true on success, false on failure.
    * @param buffer The buffer containing the encoded message.
    * @param len The length of the buffer.
    */
    virtual bool decode(const uint8_t* buffer, size_t len) = 0;
};
}  // namespace LBR
