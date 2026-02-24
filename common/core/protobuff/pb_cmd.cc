#include "pb_cmd.h"

namespace LBR
{
namespace PbCmd
{
PbCmd(size_t buffer_size, const pb_msgdesc_t* message_desc,
      void* message_struct)
    : CmdMessage(buffer_size),
      buffer_size_(buffer_size),
      message_desc_(message_desc),
      message_struct_(message_struct)
{
}

bool PbCmd::get_buf(uint8_t* buffer, size_t size)
{
    if (size < buffer_size_)
    {
        return false;  // Buffer too small
    }
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    return pb_encode(&stream, message_desc_, message_struct_);
}

size_t PbCmd::get_size() const
{
    return buffer_size_;
}

bool PbCmd::parse(uint8_t* buffer, size_t size)
{
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    return pb_decode(&stream, message_desc_, message_struct_);
}

size_t PbCmd::buffer_size() const
{
    return buffer_size_;
}

const pb_msgdesc_t* PbCmd::message_desc() const
{
    return message_desc_;
}

void* PbCmd::message_struct() const
{
    return message_struct_;
}

void PbCmd::set_message_struct(void* message_struct)
{
    message_struct_ = message_struct;
}

};  // namespace PbCmd
}  // namespace LBR