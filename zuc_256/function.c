//
// Created by 10044 on 2024/3/1.
// 简单的计算函数
//
#include"function.h"
uint32 mod_add(uint32 a, uint32 b)
{
    uint32 c = a + b;
    c = (c & 0x7fffffff) + (c >> 31);
    return c;
}

uint32 mod_2exp_mul(uint32 x, int exp)
{
    return ((x << exp) | (x >> (31 - exp))) & 0x7fffffff;
}

uint32 Rot(uint32 x, int move)
{
    return ((x << move) | (x >> (32 - move)));
}
