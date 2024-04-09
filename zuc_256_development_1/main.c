//
//
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "lib.h"
#include "function.h"
#include "key_insert.h"
#include "br.h"
#include "F_function.h"
#include "LFSRWithInitMode.h"
#include "LFSRWithWorkMode.h"

uint32 LFSR[16] = {0};
uint32 X[4] = {0};
uint32 R1 = 0, R2 = 0;
uint32 W = 0;

void Init(uint8*, uint8*);
uint32 *KeyStream_Generator(int);

int main() {
    int keylen = 0;
    uint8 key[32] = {0x00};
    uint8 iv[25] = {0x00};

    printf("请输入密钥流字长度：\n");
    scanf("%d",&keylen);

    FILE *key_in;
    key_in = fopen("key.txt","r");
    for (int i = 0; i < 32; i++)
    {   
        int t;
        fscanf(key_in,"%i",&t);
        key[i]=t;
    }
    fclose(key_in);

    FILE *iv_in;
    iv_in = fopen("iv.txt","r");
    for (int i = 0; i < 25; i++)
    {   
        int t;
        fscanf(iv_in,"%i",&t);
        iv[i]=t;
    }
    fclose(iv_in);

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);//测量开始    

    Init(key, iv);
    uint32 *keylist = KeyStream_Generator(keylen);

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    double duration = (double)(end.tv_nsec-start.tv_nsec)/((double) 1e9) + (double)(end.tv_sec-start.tv_sec);
    printf("时间为：%f\n",duration);//测量结束，显示数据

    printf("密钥字长度为：%d 密钥字流为：\n",keylen);
    for (int i = 0; i < keylen; i++)
    {
        printf("%08x\n", keylist[i]);
    }
    
    free(keylist);
    keylist = NULL;

    system("pause");
     
    return 0;
}

void Init(uint8* const k, uint8* const iv)
{
    Key_IV_Insert(k, iv, LFSR);
    R1 = R2 = 0;
    uint32 i = 0, u = 0;
    for (i = 0; i < 32; i++)
    {
        BitReconstruction(LFSR,X);
        F(&W,&R1,&R2,X);
        u = W>>1;
        LFSRWithInitMode(LFSR,u);
    }
    BitReconstruction(LFSR,X);
    F(&W,&R1,&R2,X);
    LFSRWithWorkMode(LFSR);
}

uint32 *KeyStream_Generator(int keylen)
{
    uint32 Z = 0, i = 0;
    uint32 *keystream = (uint32 *)malloc(keylen * sizeof(uint32));
    for (i = 0; i < keylen; i++)
    {
        BitReconstruction(LFSR,X);
        F(&W,&R1,&R2,X);
        keystream[i] = W ^ X[3];
        LFSRWithWorkMode(LFSR);
    }
    return keystream;
}