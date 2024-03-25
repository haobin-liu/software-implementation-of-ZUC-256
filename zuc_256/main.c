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

    struct stat buf;
    stat("m.txt", &buf);
    if (buf.st_size * 8 % 32 == 0)
    {
        keylen = buf.st_size * 8 / 32;
    }
    else
    {
        keylen = buf.st_size * 8 / 32 + 1;
    }
    
    //测量时间的函数调用
    struct timespec start;//读取开始时间
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    Init(key, iv);

    struct timespec end;//读取结束时间
    clock_gettime(CLOCK_MONOTONIC, &end);
    double duration = (double)(end.tv_nsec-start.tv_nsec)/((double)1e9)+(double)(end.tv_sec-start.tv_sec);
    printf("初始化时间为：%f\n",duration);
    
    uint32 *keylist = KeyStream_Generator(keylen);
    uint32 *cipher = (uint32 *)malloc(keylen * sizeof(uint32));
    uint32 *u_cipher = (uint32 *)malloc(keylen * sizeof(uint32));
    uint32 *m = (uint32 *)malloc(keylen * sizeof(uint32));

    printf("密钥字长度为：%d 密钥字流为：\n",keylen);
    for (int i = 0; i < keylen; i++)
    {
        printf("%08x\n", keylist[i]);
    }
    
    //读取文件
    FILE *m_in;//读取文件的值放入32bit存储空间中
    m_in = fopen("m.txt","r");
    fread(m,sizeof(uint32),keylen,m_in);
    fclose(m_in);    

    //加密
    printf("加密后的文件为：cipher.txt\n");
    for (int i = 0; i < keylen; i++)
    {
        cipher[i] = keylist[i] ^ m[i];
    }
    FILE *cipher_file;
    cipher_file = fopen("cipher.txt","w");
    fwrite(cipher,sizeof(uint32),keylen,cipher_file);
    fclose(cipher_file);
    
    //解密
    printf("解密后的文件为：u_cipher.txt\n");
    for (int i = 0; i < keylen; i++)
    {
        u_cipher[i] = keylist[i] ^ cipher[i];
    }
    FILE *u_cipher_file;
    u_cipher_file = fopen("u_cipher.txt","w");
    fwrite(u_cipher,sizeof(uint32),keylen,u_cipher_file);
    fclose(u_cipher_file);

    free(keylist);
    keylist = NULL;
    free(cipher);
    cipher = NULL;
    free(u_cipher);
    u_cipher =NULL;
    free(m);
    m = NULL;

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
