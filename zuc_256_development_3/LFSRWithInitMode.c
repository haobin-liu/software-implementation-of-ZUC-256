//
// Created by 10044 on 2024/3/1.
// LFSR初始工作模式函数
//

#include "LFSRWithInitMode.h"
void LFSRWithInitMode(uint32* const LFSR, uint32 u)
{
    uint32 v = 0, i = 0, s16=0;
    uint64 tmp = 0;

    tmp = ((uint64)LFSR[15]<<15)+((uint64)LFSR[13]<<17)+((uint64)LFSR[10]<<21)+((uint64)LFSR[4]<<20)+((uint64)LFSR[0]<<8)+(uint64)LFSR[0];
    //tmp = (tmp & 0x7fffffff) + (tmp >> 31);
    v = (tmp & 0x7fffffff) + (tmp >> 31);

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