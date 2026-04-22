#include "pb_cmd.h"

namespace LBR
{

PbCmd::~PbCmd()
{
}

int PbCmd::encode(uint8_t* buffer, size_t maxlen) const
{
    // stream binary data into the buffer, and returns the number of bytes written or -1 on failure.
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, maxlen);

    if (!pb_encode(&stream, RocketStructProto_fields, &msg))
    {
        return -1;  // Encoding failed
    }
    return stream.bytes_written;
}

bool PbCmd::decode(const uint8_t* buffer, size_t len)
{
    // stream binary data from the buffer, and returns true on success, false on failure.
    pb_istream_t stream = pb_istream_from_buffer(buffer, len);

    return pb_decode(&stream, RocketStructProto_fields, &msg);
}

}  // namespace LBR
