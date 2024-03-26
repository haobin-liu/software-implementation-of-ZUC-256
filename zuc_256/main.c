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
    
    //读取key
    FILE *key_in;
    key_in = fopen("key.txt","r");
    for (int i = 0; i < 32; i++)
    {   
        int t;
        fscanf(key_in,"%i",&t);
        key[i]=t;
    }
    fclose(key_in);
    
    //读取iv
    FILE *iv_in;
    iv_in = fopen("iv.txt","r");
    for (int i = 0; i < 25; i++)
    {   
        int t;
        fscanf(iv_in,"%i",&t);
        iv[i]=t;
    }
    fclose(iv_in);
    
    //计算要生成的密钥字长度
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
    
    //生成密钥字
    Init(key, iv);
    uint32 *keylist = KeyStream_Generator(keylen);
    printf("密钥字长度为：%d 密钥字流为：\n",keylen);
    for (int i = 0; i < keylen; i++)
    {
        printf("%08x\n", keylist[i]);
    }
    
    //将keylist转换为一个8位的数组
    uint8 *keylist_1 = (uint8 *)keylist;
    // for (int  i = 0; i < keylen*4; i++)
    // {
    //     printf("%02x\n", keylist_1[i]);
    // }
    
    //定义区
    uint8 *m = (uint8 *)malloc(buf.st_size * sizeof(uint8));
    uint8 *cipher = (uint8 *)malloc(buf.st_size * sizeof(uint8));
    uint8 *u_cipher = (uint8 *)malloc(buf.st_size * sizeof(uint8));
    
    //读取文件
    FILE *m_in;//读取文件的值放入32bit存储空间中
    m_in = fopen("m.txt","r");
    fread(m,sizeof(uint8),buf.st_size,m_in);
    fclose(m_in);    

    //加密
    printf("加密后的文件为：cipher.txt\n");
    for (int i = 0; i < buf.st_size; i++)
    {
        cipher[i] = keylist_1[i] ^ m[i];
    }
    FILE *cipher_file;
    cipher_file = fopen("cipher.txt","w");
    fwrite(cipher,sizeof(uint8),buf.st_size,cipher_file);
    fclose(cipher_file);
    
    //解密
    printf("解密后的文件为：u_cipher.txt\n");
    for (int i = 0; i < buf.st_size; i++)
    {
        u_cipher[i] = keylist_1[i] ^ cipher[i];
    }
    FILE *u_cipher_file;
    u_cipher_file = fopen("u_cipher.txt","w");
    fwrite(u_cipher,sizeof(uint8),buf.st_size,u_cipher_file);
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
