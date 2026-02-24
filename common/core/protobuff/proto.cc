// This is a test file for the protobuff library.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pb_cmd.h"
#include "protobuff.pb.h"

using namespace LBR;
int main()
{
    // Buffer to hold the encoded data
    uint8_t buffer[128];
    // Create PbCmd instances for encoding and decoding
    PbCmd tx_cmd, rx_cmd;

    // Create and fill the PbCmd message
    strcpy(tx_cmd.msg.Name, "Bex Saw");
    tx_cmd.msg.Year = 2026;
    strcpy(tx_cmd.msg.clubName, "Long Beach Rocketry");
    tx_cmd.msg.memberCount = 1;

    // Encode the message into the buffer
    int len = tx_cmd.encode(buffer, sizeof(buffer));
    if (len <= 0)
    {
        return 1;
    }

    // Decode the message from the buffer
    if (rx_cmd.decode(buffer, len))
    {
        printf("Decoded message:\n");
        printf("Name: %s\n", rx_cmd.msg.Name);
        printf("Year: %ld\n", (long)rx_cmd.msg.Year);
        printf("Club Name: %s\n", rx_cmd.msg.clubName);
        printf("Member Count: %ld\n", (long)rx_cmd.msg.memberCount);
    }
    else
    {
        printf("Decoding failed\n");
        return 1;
    }

    return 0;
}