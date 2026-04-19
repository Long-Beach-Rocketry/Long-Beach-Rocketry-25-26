
// Testing USART with protobuf messages using the helper functions defined in pb_helper.h
#include <cstdio>
#include <cstring>
#include "board.h"
#include "pb_cmd.h"
#include "pb_helper.h"

using namespace LBR;

int main(int argc, char** argv)
{
    bsp_init();
    Board board = get_board();

    // Create and fill a PbCmd message
    PbCmd tx_cmd;
    PbCmd rx_cmd;

    // Fill the message with test data
    strcpy(tx_cmd.msg.Name, "HELLO");
    tx_cmd.msg.Year = 2026;
    strcpy(tx_cmd.msg.clubName, "LBR");
    tx_cmd.msg.memberCount = 1;

    // Send & receive protobuf message over USART
    send_cmd_msg(&tx_cmd, board.usart);
    receive_cmd_msg(&rx_cmd, board.usart);

    // Print received data to verify correct encoding/decoding
    printf("Received: %s %ld %s %ld\n", rx_cmd.msg.Name, (long)rx_cmd.msg.Year,
           rx_cmd.msg.clubName, (long)rx_cmd.msg.memberCount);

    while (1)
    {
    }

    return 0;
}
