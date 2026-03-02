
// Testing USART with protobuf messages using the helper functions defined in usart_pb_helper.h
#include <cstdio>
#include <cstring>
#include "board.h"
#include "pb_cmd.h"
#include "usart_pb_helper.h"

using namespace LBR;

int main(int argc, char** argv)
{
    bsp_init();
    Board board = get_board();

    // Create and fill a PbCmd message
    PbCmd tx_cmd;
    strcpy(tx_cmd.msg.Name, "BEX ABB");
    tx_cmd.msg.Year = 2026;
    strcpy(tx_cmd.msg.clubName, "LBR");
    tx_cmd.msg.memberCount = 1;

    // Send the protobuf message over USART
    send_cmd_msg(&tx_cmd, board.usart);

    // Receive a protobuf message over USART (example)
    PbCmd rx_cmd;
    receive_cmd_msg(&rx_cmd, board.usart);

    // Print received data to verify correct encoding/decoding
    printf("Received: %s %ld %s %ld\n", rx_cmd.msg.Name, (long)rx_cmd.msg.Year,
           rx_cmd.msg.clubName, (long)rx_cmd.msg.memberCount);

    while (1)
    {
        // Main loop can be extended as needed
    }

    return 0;
}
