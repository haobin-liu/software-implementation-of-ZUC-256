//
// Created by 10044 on 2024/3/1.
//

#include"lib.h"
#include"LFSRWithWorkMode.h"
void LFSRWithWorkMode(uint32* const LFSR)
{
    uint32 s16 = 0, i = 0;
    uint64 tmp = 0;

    tmp = ((uint64)LFSR[15]<<15)+((uint64)LFSR[13]<<17)+((uint64)LFSR[10]<<21)+((uint64)LFSR[4]<<20)+((uint64)LFSR[0]<<8)+(uint64)LFSR[0];
    //tmp = (tmp & 0x7fffffff) + (tmp >> 31);
    s16 = (tmp & 0x7fffffff) + (tmp >> 31);

    if (s16 == 0)
        s16 = 0x7fffffff;

    for (i = 0; i < 15; i++)
    {
        LFSR[i] = LFSR[i + 1];
    }    
    LFSR[15] = s16;
}