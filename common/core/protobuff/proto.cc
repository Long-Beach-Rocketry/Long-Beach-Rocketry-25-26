/**
 * @note This is a test file for the protobuff library. 
 * It encodes and decodes a simple RocketStructProto
 * -message and prints the result.
*/

#include <cstdio>
#include <cstring>
#include "pb_cmd.h"
#include "protobuff.pb.h"

using namespace std;

namespace LBR
{
int main()
{
    // Buffer to hold the encoded data
    uint8_t buffer[128];

    // Initialize the RocketStructProto message
    RocketStructProto rocket = RocketStructProto_init_zero;

    // Fill the RocketStructProto message with data
    strcpy(rocket.Name, "ROCKET");
    rocket.Year = 2026;
    strcpy(rocket.clubName, "LBR");
    rocket.memberCount = 1;

    // Create PbCmd wrapper (as CmdMessage pointer for interface demonstration)
    CmdMessage* cmd =
        new PbCmd(sizeof(buffer), RocketStructProto_fields, &rocket);

    // Encode the message using CmdMessage interface
    if (!cmd->get_buf(buffer, sizeof(buffer)))
    {
        printf("Encoding failed\n");
        delete cmd;
        return 1;
    }

    // Zero out the struct to simulate receiving and decoding
    memset(&rocket, 0, sizeof(rocket));
    // Set the struct pointer again for decoding
    static_cast<PbCmd*>(cmd)->set_message_struct(&rocket);

    // Decode the message using CmdMessage interface
    if (cmd->parse(buffer, cmd->get_size()))
    {
        printf("Decoded message:\n");
        printf("Name: %s\n", rocket.Name);
        printf("Year: %d\n", rocket.Year);
        printf("Club Name: %s\n", rocket.clubName);
        printf("Member Count: %d\n", rocket.memberCount);
    }
    else
    {
        printf("Decoding failed\n");
        delete cmd;
        return 1;
    }

    delete cmd;
}
}  // namespace LBR