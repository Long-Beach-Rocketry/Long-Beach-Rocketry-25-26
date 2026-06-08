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

    // Counter for different packet values
    uint32_t count = 0;

    // Fill the message with test data
    tx.msg = RocketMessage_init_default;
    tx.msg.has_header = true;
    tx.msg.header.device_id = 0x67;
    tx.msg.header.packet_count = count++;
    tx.msg.header.timestamp_ms = 0;

    // Send & receive protobuf message over USART
    pb.send(&tx, board.usart);
    pb.receive(&rx, board.usart);

    // Print received data to verify correct encoding/decoding
    printf("Received: device=0x%lx packet=%lu ts=%lu\n",
           (unsigned long)rx.msg.header.device_id,
           (unsigned long)rx.msg.header.packet_count,
           (unsigned long)rx.msg.header.timestamp_ms);

    while (1)
    {
    }

    return 0;
}
