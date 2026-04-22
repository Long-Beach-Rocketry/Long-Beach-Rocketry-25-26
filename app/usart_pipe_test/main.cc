#include <cstdio>
#include <cstring>
#include "board.h"
#include "usart_pipe.h"

using namespace LBR;

/**
* @note The idea behind it is:
*       Fill struct → encode with nanopb → tx_bf bytestream → rs485 → 
*                     receive tx_buffer → check if get all tx_buffer → rx_buffer 
*/

// Testing USART with nanopb encoding/decoding for Board-To-Board communication.
uint8_t rxb;

int main(int argc, char** argv)
{
    bsp_init();
    Board board = get_board();

    // Create and fill a PbCmd message
    PbCmd tx;
    PbCmd rx;

    // Create a Pipeline instance
    Pipeline pb;

    // Counter for different timestamp values
    int32_t count = 0;

    // Fill the message with test data
    strcpy(tx.msg.Name, "LBR");
    tx.msg.Year = 2026;
    strcpy(tx.msg.clubName, "LBR");
    tx.msg.timestamp_ms = count++;  // Increment timestamp for each message

    // Send & receive protobuf message over USART
    pb.send(&tx, board.usart);
    pb.receive(&rx, board.usart);

    // Print received data to verify correct encoding/decoding
    printf("Received: %s %ld %s %ld\n", rx.msg.Name, (long)rx.msg.Year,
           rx.msg.clubName, (long)rx.msg.timestamp_ms);

    while (1)
    {
    }

    return 0;
}
