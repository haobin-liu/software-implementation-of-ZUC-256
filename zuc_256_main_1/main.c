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

uint32 Tag[4]={0};

void Init(uint8*, uint8*);
uint32 *KeyStream_Generator(int);
uint32 *MC_Generator_32(int, uint8*);
uint32 *MC_Generator_64(int, uint8*);
uint32 *MC_Generator_128(int, uint8*);

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
    
    Init(key, iv);//初始化
    
    //生成标签
    uint8 m_test[50]={0};
    for (int i = 0; i < 500; i++)
    {
        m_test[i] = 0x11;
    }
    uint32 *MC_list = MC_Generator_32(500,m_test);
    for (int i = 0; i < 1; i++)
    {
        printf("结果为：%08x\n", MC_list[i]);
    }
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

uint32 *MC_Generator_32(int l, uint8* m)
{
    int t = 32;
    int L = 0;
    if (l * 8 % 32 == 0)
    {
        L = l * 8 / 32 + 2 * t / 32;
    }
    else
    {
        L = l * 8 / 32 + 1 + 2 * t / 32;
    }
    uint32 *keylist = KeyStream_Generator(L);
    
    printf("密钥字长度为：%d 密钥字流为：\n",L);
    for (int i = 0; i < L; i++)
    {
        printf("%08x\n", keylist[i]);
    }

    uint32 W = 0;
    //W1 = 0, W2 = 0
    int n_1=0,n_2=0,n_3=0,n_4=0;
    uint32* MC_List = &Tag[0];//取地址，方便返回值
    for (int i = 0; i < 1; i++)
    {
        Tag[i] = keylist[i];
    }
    
    for (unsigned int i = 0; i < l*8-1; i++)
    {
        n_1 = (t+i)/32;//控制32 bit数组的位置
        n_2 = (t+i)%32;//控制32 bit单个bit位置
        n_3 = i/8;//控制8 bit数组的位置
        n_4 = i%8;//控制8 bit单个bit位置
        if (n_2 == 0)
        {
            W = keylist[n_1];
        }
        else
        {
            W = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
        }
        //W1 = (keylist[n_1]<<n_2);
        //W2 = (keylist[n_1+1]>>(32-n_2));
        
        if ((m[n_3]<<n_4) & 0x80)
        {
            Tag[0] = Tag[0] ^ W;
        }
    }
    
    n_1 = (t+l*8)/32;
    n_2 = (t+l*8)%32;
    if (n_2 == 0)
    {
        W = keylist[n_1];
    }
    else
    {
        W = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
    }
    Tag[0] = Tag[0] ^ W;
    return MC_List;

}

uint32 *MC_Generator_64(int l, uint8* m)
{
    int t = 64;
    int L = 0;
    if (l * 8 % 32 == 0)
    {
        L = l * 8 / 32 + 2 * t / 32;
    }
    else
    {
        L = l * 8 / 32 + 1 + 2 * t / 32;
    }
    uint32 *keylist = KeyStream_Generator(L);
    
    printf("密钥字长度为：%d 密钥字流为：\n",L);
    for (int i = 0; i < L; i++)
    {
        printf("%08x\n", keylist[i]);
    }
    
    uint32 W[2] = {0};
    int n_1=0,n_2=0,n_3=0,n_4=0;
    uint32* MC_List = &Tag[0];
    for (int i = 0; i < 2; i++)
    {
        Tag[i] = keylist[i];
    }
    
    for (unsigned int i = 0; i < l*8-1; i++)
    {
        n_1 = (t+i)/32;//控制32 bit数组的位置
        n_2 = (t+i)%32;//控制32 bit单个bit位置
        n_3 = i/8;//控制8 bit数组的位置
        n_4 = i%8;//控制8 bit单个bit位置
        if (n_2 == 0)
        {
            W[0] = keylist[n_1];
            W[1] = keylist[n_1+1];
        }
        else
        {
            W[0] = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
            W[1] = (keylist[n_1+1]<<n_2)|(keylist[n_1+2]>>(32-n_2));
        }
        //W1 = (keylist[n_1]<<n_2);
        //W2 = (keylist[n_1+1]>>(32-n_2));

        if ((m[n_3]<<n_4) & 0x80)
        {
            Tag[0] = Tag[0] ^ W[0];
            Tag[1] = Tag[1] ^ W[1];
        }
    }
        
    n_1 = (t+l*8)/32;
    n_2 = (t+l*8)%32;
    if (n_2 == 0)
    {
        W[0] = keylist[n_1];
        W[1] = keylist[n_1+1];
    }
    else
    {
        W[0] = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
        W[1] = (keylist[n_1+1]<<n_2)|(keylist[n_1+2]>>(32-n_2));
    }
    Tag[0] = Tag[0] ^ W[0];
    Tag[1] = Tag[1] ^ W[1];
    return MC_List;  
}

uint32 *MC_Generator_128(int l, uint8* m)
{
    int t = 128;
    int L = 0;
    if (l * 8 % 32 == 0)
    {
        L = l * 8 / 32 + 2 * t / 32;
    }
    else
    {
        L = l * 8 / 32 + 1 + 2 * t / 32;
    }
    uint32 *keylist = KeyStream_Generator(L);
    
    printf("密钥字长度为：%d 密钥字流为：\n",L);
    for (int i = 0; i < L; i++)
    {
        printf("%08x\n", keylist[i]);
    }
    
    uint32 W[4] = {0};
    int n_1=0,n_2=0,n_3=0,n_4=0;
    uint32* MC_List = &Tag[0];
    for (int i = 0; i < 4; i++)
    {
        Tag[i] = keylist[i];
    }
    
    for (unsigned int i = 0; i < l*8-1; i++)
    {
        n_1 = (t+i)/32;//控制32 bit数组的位置
        n_2 = (t+i)%32;//控制32 bit单个bit位置
        n_3 = i/8;//控制8 bit数组的位置
        n_4 = i%8;//控制8 bit单个bit位置
        if (n_2 == 0)
        {
            W[0] = keylist[n_1];
            W[1] = keylist[n_1+1];
            W[2] = keylist[n_1+2];
            W[3] = keylist[n_1+3];
        }
        else
        {
            W[0] = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
            W[1] = (keylist[n_1+1]<<n_2)|(keylist[n_1+2]>>(32-n_2));
            W[2] = (keylist[n_1+2]<<n_2)|(keylist[n_1+3]>>(32-n_2));
            W[3] = (keylist[n_1+3]<<n_2)|(keylist[n_1+4]>>(32-n_2));
        }
        //W1 = (keylist[n_1]<<n_2);
        //W2 = (keylist[n_1+1]>>(32-n_2));

        if ((m[n_3]<<n_4) & 0x80)
        {
            Tag[0] = Tag[0] ^ W[0];
            Tag[1] = Tag[1] ^ W[1];
            Tag[2] = Tag[2] ^ W[2];
            Tag[3] = Tag[3] ^ W[3];
        }
    }
        
    n_1 = (t+l*8)/32;
    n_2 = (t+l*8)%32;
    if (n_2 == 0)
    {
        W[0] = keylist[n_1];
        W[1] = keylist[n_1+1];
        W[2] = keylist[n_1+2];
        W[3] = keylist[n_1+3];
    }
    else
    {
        W[0] = (keylist[n_1]<<n_2)|(keylist[n_1+1]>>(32-n_2));
        W[1] = (keylist[n_1+1]<<n_2)|(keylist[n_1+2]>>(32-n_2));
        W[2] = (keylist[n_1+2]<<n_2)|(keylist[n_1+3]>>(32-n_2));
        W[3] = (keylist[n_1+3]<<n_2)|(keylist[n_1+4]>>(32-n_2));
    }
    Tag[0] = Tag[0] ^ W[0];
    Tag[1] = Tag[1] ^ W[1];
    Tag[2] = Tag[2] ^ W[2];
    Tag[3] = Tag[3] ^ W[3];
    return MC_List;  
}