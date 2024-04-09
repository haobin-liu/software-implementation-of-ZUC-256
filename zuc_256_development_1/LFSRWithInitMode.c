//
// Created by 10044 on 2024/3/1.
// LFSR初始工作模式函数
//

#include "LFSRWithInitMode.h"
void LFSRWithInitMode(uint32* const LFSR, uint32 u)
{
    uint32 v = 0, tmp = 0, i = 0, s16=0;

    v = LFSR[0];//s0

    tmp = mod_2exp_mul(LFSR[0], 8);
    v = mod_add(v, tmp);//s0+2^8*s0

    tmp = mod_2exp_mul(LFSR[4], 20);
    v = mod_add(v, tmp);//s0+2^8*s0+2^20*s4

    tmp = mod_2exp_mul(LFSR[10], 21);
    v = mod_add(v, tmp);//s0+2^8*s0+2^20*s4+2^21*s10

    tmp = mod_2exp_mul(LFSR[13], 17);
    v = mod_add(v, tmp);//s0+2^8*s0+2^20*s4+2^21*s10+2^17*s13

    tmp = mod_2exp_mul(LFSR[15], 15);
    v = mod_add(v, tmp);//s0+2^8*s0+2^20*s4+2^21*s10+2^17*s13+2^15*s15

    if (v == 0)
    {
        v = 0x7fffffff;
    }
    
    s16 = mod_add(v, u);
    
    if (s16 == 0)
    {
        s16 = 0x7fffffff;
    }
    
    for (i = 0; i < 15; i++)
    {
        LFSR[i] = LFSR[i + 1];
    }
    LFSR[15] = s16;
}