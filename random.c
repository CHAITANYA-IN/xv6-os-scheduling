#include "types.h"
/* 
Reference : https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Galois_LFSRs
LFSR(x^19 + x^18 + x^17 + x^14 + 1)
*/
uint randtickets(uint max_tickets)
{
    static uint start_state = 0xACE1u;
    uint lfsr;
    uint bit;
    uint period = 0;
    if (start_state == 0)
        start_state++;
    lfsr = start_state++;
    do
    {
        bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 2) ^ (lfsr >> 5));
        lfsr = (lfsr >> 1) | (bit << 15);
        period++;
    } while (period < max_tickets);
    return (lfsr % max_tickets);
}