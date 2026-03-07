#include "st_usart.h"

namespace LBR
{
namespace Stmh7
{
    StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clck, 
        uint32_t baud_rate) : base_addr(base_addr), usartdiv(sys_clck / baud_rate) {}

    bool StUsart::send(std::span<const uint8_t> txbuf) {
        
    }

    bool StUsart::receive(uint8_t& byte) {
    
    }

    bool StUsart::init() {

    }

    USART_TypeDef* StUsart::get_addr() {
        return this->base_addr;
    }

}
}