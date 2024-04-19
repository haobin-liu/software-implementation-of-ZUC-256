//
// Created by 10044 on 2024/3/1.
//

#include"lib.h"
#include"LFSRWithWorkMode.h"
void LFSRWithWorkMode(uint32* const LFSR)
{
    uint32 s16 = 0, tmp = 0, i = 0;

    s16 = LFSR[0];//s0

    tmp = mod_2exp_mul(LFSR[0], 8);
    s16 = mod_add(s16, tmp);//s0+2^8*s0

    tmp = mod_2exp_mul(LFSR[4], 20);
    s16 = mod_add(s16, tmp);//s0+2^8*s0+2^20*s4

    tmp = mod_2exp_mul(LFSR[10], 21);
    s16 = mod_add(s16, tmp);//s0+2^8*s0+2^20*s4+2^21*s10

    tmp = mod_2exp_mul(LFSR[13], 17);
    s16 = mod_add(s16, tmp);//s0+2^8*s0+2^20*s4+2^21*s10+2^17*s13

    tmp = mod_2exp_mul(LFSR[15], 15);
    s16 = mod_add(s16, tmp);//s0+2^8*s0+2^20*s4+2^21*s10+2^17*s13+2^15*s15

    if (s16 == 0)
        s16 = 0x7fffffff;

    for (i = 0; i < 15; i++)
    {
        LFSR[i] = LFSR[i + 1];
    }    
    LFSR[15] = s16;
}