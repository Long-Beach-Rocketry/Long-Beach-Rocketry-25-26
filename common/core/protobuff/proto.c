#include <stdio.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "protobuff.pb.h"

int main()
{
    uint8_t buffer[128];

    RocketStructProto rocket = RocketStructProto_init_zero;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    strcpy(rocket.Name, "ROCKET");
    rocket.Year = 2025;
    strcpy(rocket.clubName, "LBR");
    rocket.memberCount = 1;

    bool status = pb_encode(&stream, RocketStructProto_fields, &rocket);
    // bool read = pb_read(&stream, buffer, sizeof(buffer));
    if (status)
    {
        printf("%s", buffer);
    }
}
