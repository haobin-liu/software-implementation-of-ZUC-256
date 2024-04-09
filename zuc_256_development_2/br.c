//
// Created by 10044 on 2024/3/1.
// 比特重组时，S中有31位，低位对齐，重组后为32位
//

#include "br.h"

void BitReconstruction(uint32* const LFSR,uint32* const X)
{
    X[0] = ((LFSR[15] & 0x7fff8000) << 1) | (LFSR[14] & 0xffff);
    X[1] = (LFSR[11] << 16) | (LFSR[9] >> 15);
    X[2] = (LFSR[7] << 16) | (LFSR[5] >> 15);
    X[3] = (LFSR[2] << 16) | (LFSR[0] >> 15);
}