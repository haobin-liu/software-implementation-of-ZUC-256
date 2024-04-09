//
// Created by 10044 on 2024/3/2.
//
#include "F_function.h"

uint32 S(uint32 a)
{
    uint16 x[2] = {0}, y[2] = {0};
    uint32 b = 0;
    x[0] = a >> 16;
    x[1] = a & 0xffff;
    y[0] = S_big[x[0]];
    y[1] = S_big[x[1]];
    b = (y[0] << 16) | y[1];
    return b;
}

uint32 L1(uint32 x)
{
    return (x ^ Rot(x, 2) ^ Rot(x, 10) ^ Rot(x, 18) ^ Rot(x, 24));
}

uint32 L2(uint32 x)
{
    return (x ^ Rot(x, 8) ^ Rot(x, 14) ^ Rot(x, 22) ^ Rot(x, 30));
}

void F(uint32 *W, uint32 *R1, uint32 *R2, uint32* const X)
{
    uint32 W1 = 0, W2 = 0;
    uint32 tmp1 = 0, tmp2 = 0;
    *W = (X[0] ^ (*R1)) + (*R2);
    W1 = (*R1) + X[1];
    W2 = (*R2) ^ X[2];
    *R1 = S(L1((W1 << 16) | (W2 >> 16)));
    *R2 = S(L2((W2 << 16) | (W1 >> 16)));
}