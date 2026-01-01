#include "pps.h"

int main() {
    PpsContext pps;
    ppsInit(pps);
    while (true) {
        ppsUpdate(pps);
        // Add delay or scheduling as needed
    }
    return 0;
}
