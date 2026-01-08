/**
 * @file main.cc
 * @brief PPS module test application
 * @note This application tests the PPS module functionality.
 * @author Bex Saw
 * @date 2025/12/31
 */

#include "pps.h"


using LBR::Pps;

int main() {
    Pps pps;
    while (true) {
        pps.update();
        // Add scheduling delay (come with testing prototype)
    }   
    return 0;
}