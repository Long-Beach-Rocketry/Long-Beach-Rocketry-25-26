
#pragma once
#include <pb_decode.h>
#include <pb_encode.h>
#include <cstddef>
#include <cstdint>
#include "cmd_msg.h"

namespace LBR
{

/**
* @note PdCmd inherits the CmdMessage interface and provides methods 
* for encoding and decoding protobuf messages. It uses the nanopb library 
* for serialization and deserialization.
*/

class PbCmd : public CmdMessage
{
public:
    // Constructor
    explicit PbCmd(size_t buffer_size, const pb_msgdesc_t* message_desc,
                   void* message_struct);

    // Destructor for cleanup if necessary
    ~PbCmd() = default;

    // Serialization interface
    bool get_buf(uint8_t* buffer, size_t size)
        override;  // Encode the message into the provided buffer
    size_t get_size()
        const override;  // Return the size of the serialized message
    bool parse(uint8_t* buffer, size_t size) override;

    // Accessors
    size_t buffer_size() const;  // Getter for the buffer size
    const pb_msgdesc_t* message_desc()
        const;                     // Getter for the message descriptor
    void* message_struct() const;  // Getter for the message struct pointer
    void set_message_struct(
        void* message_struct);  // Setter for the message struct pointer

private:
    size_t buffer_size_;
    const pb_msgdesc_t* message_desc_;
    void* message_struct_;
};
}  // namespace LBR