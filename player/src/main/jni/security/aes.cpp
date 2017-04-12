#include "security/aes.h"

AES::AES() {
    memcpy(Sbox, sBox, 256);
    memcpy(InvSbox, invsBox, 256);
}

AES::~AES() {
    free(Sbox);
    free(InvSbox);
}

int AES::setKey(unsigned char *key) {
    memset((void *) w, 0, sizeof(w));
    KeyExpansion(key, w);
    return 1;
}

int AES::getKey_A(unsigned char *key) {
    double e = 1, index;
    int ii = 1, jj = 1;
    do {
        ii *= jj;
        index = 1.0 / (double) ii;
        e += index;
        jj++;
    }
    while (index >= 1e-10);
    unsigned char *key_1 = (unsigned char *) malloc(21 * sizeof(unsigned char));
    if (key_1 == NULL) {
        LOGE("[AES:getKey_A]malloc key_1 error!");
        return -1;
    }
    memset((void *) key_1, 0, 21 * sizeof(unsigned char));
    sprintf((char *) key_1, "%0.16f\n", e);
    *key = *key_1;
    memcpy((void *) (key + 1), (void *) (key_1 + 2),
           15 * sizeof(unsigned char)); //(void *) *sizeof(char)
    *(key + 17) = 0;
    free(key_1);
    return 0;
}

int AES::encode(const char *in, char *out) {
    //设置 key
    unsigned char *key = (unsigned char *) malloc(17 * sizeof(unsigned char));
    if (key == NULL) {
        LOGE("[AES:encode]malloc key error!");
        return -1;
    }
    memset((void *) key, 0, 17 * sizeof(unsigned char));
    getKey_A(key);
    setKey(key);
    memset((void *) key, 0, sizeof(key));
    free(key);
    //加密 lisense
    int len = strlen(in);
    char *mingwen = (char *) malloc((len + 1) * sizeof(char));
    if (mingwen == NULL) {
        LOGE("[AES:encode]malloc mingwen %d error!", (len + 1));
        return -1;
    }
    memset((void *) mingwen, 0, (len + 1) * sizeof(char));
    if (len == 16) {
        memcpy((void *) mingwen, (void *) in, len * sizeof(char));
    } else {
        free(mingwen);
        LOGE("[AES:encode]input format error, len is %d, input:%s!", len, in);
        return -1;
    }
    Bm53Cipher(mingwen, out);
    free(mingwen);
    return 0;
}

//从服务器解密，并且验证
int AES::InvCipher_server(const char *lisense,
                          const char *android_ID,
                          const char *ciphertext_0,
                          const char *ciphertext_1) {
    //1. 将 Android ID 设置成密钥
    int len;
    len = strlen(android_ID);
    if (len == 16) {
        unsigned char *key = (unsigned char *) malloc((len + 1) * sizeof(unsigned char));
        if (key == NULL) {
            LOGE("[AES:InvCipher_server]malloc %d error! \n", (len + 1));
            return -1;
        }
        memset((void *) key, 0, (len + 1) * sizeof(unsigned char));
        memcpy((void *) key, (void *) android_ID, len * sizeof(char));
        setKey(key);
        memset((void *) key, 0, (len + 1) * sizeof(unsigned char));
        free(key);
    }
    else {
        LOGI("[AES:InvCipher_server]android_ID format error, len is %d!", len);
        return -2;
    }
    //2. 用 Android ID 解密 密钥B
    len = strlen(ciphertext_1);
    char *miwen = (char *) malloc((len + 1) * sizeof(char));        //根据密文长度申请密文空间
    if (miwen == NULL) {
        LOGE("[AES:InvCipher_server]malloc miwen %d error! \n", (len + 1));
        return -1;
    }
    memset((void *) miwen, 0, (len + 1) * sizeof(char));
    memcpy((void *) miwen, (void *) ciphertext_1, len * sizeof(char)); //复制密文

    len = 64;        //申请明文需要空间，暂时设定为 256 +1 个
    char *mingwen = (char *) malloc((len + 1) * sizeof(char));        //申请明文空间
    if (mingwen == NULL) {
        LOGE("[AES:InvCipher_server]malloc mingwen %d error! \n", (len + 1));
        return -1;
    }
    memset((void *) mingwen, 0, (len + 1) * sizeof(char));
    memset((void *) mingwen, 1, (16) * sizeof(char));
    Bm53InvCipher(miwen, mingwen);    //解密得到 KEY_1
    free(miwen);

    //3.  密钥B 解密 License
    //设置 key
    len = strlen(mingwen);        //明文长度，看是否是正确的 密钥长度
    if (len == 16) {
        setKey((unsigned char *) mingwen);
    } else {
        LOGI("[AES:InvCipher_server]KEY_B format error, len is %d!", len);
        return -2;
    }
    free(mingwen);
    //根据密文长度申请密文空间
    len = strlen(ciphertext_0);
    char *miwen_1 = (char *) malloc((len + 1) * sizeof(char));
    if (miwen_1 == NULL) {
        LOGE("[AES:InvCipher_server]malloc miwen_1 %d error! \n", (len + 1));
        return -1;
    }
    memset((void *) miwen_1, 0, (len + 1) * sizeof(char));
    memcpy((void *) miwen_1, (void *) ciphertext_0, len * sizeof(char)); //复制密文

    len = 64;        //申请明文需要空间，暂时设定为 256 +1 个
    char *mingwen_1 = (char *) malloc((len + 1) * sizeof(char));        //申请明文空间
    if (mingwen_1 == NULL) {
        LOGE("[AES:InvCipher_server]malloc mingwen_1 %d error! \n", (len + 1));
        return -1;
    }
    memset((void *) mingwen_1, 1, (len + 1) * sizeof(char));
    Bm53InvCipher(miwen_1, mingwen_1);
    free(miwen_1);
    int s = memcmp(mingwen_1, lisense, 16);
    if (s == 0) {
        free(mingwen_1);
        return 0;
    }
    else {
        free(mingwen_1);
        return s;
    }
}

//int ciphertext(const char* lisense,
//               const char* android_ID,
//                char* ciphertext_0,
//                char* ciphertext_1)
//{
//    char mingwen[128] = {0};
//    unsigned char key[128] = {0};// "2718281827117589";
//    getKey_A(key);
//    AES(key);
//    memset((void*)key, 0, sizeof(key));		//使用完 密钥，销毁
//    //加密 lisense
//    int len = strlen(lisense);
//    if( len == 16)
//    {
//        memcpy((void *)mingwen, (void *)lisense, len*sizeof(char));
//    }
//    else
//    {
//        LOGI("Lisense format error!");
//        return -1;
//    }
//    Bm53Cipher(mingwen, ciphertext_0);
//    //加密 Android ID
//    len = strlen(android_ID);
//    if( len == 16)
//    {
//        memcpy((void *)mingwen, (void *)android_ID, len*sizeof(char));
//    }
//    else
//    {
//        LOGI("android_ID format error!");
//        return -2;
//    }
//    Bm53Cipher(mingwen, ciphertext_1);
//    return 0;
//}

////模拟服务器加密结果
//int ciphertext_server( const char* lisense,
//                       const char* android_ID,
//                        char* ciphertext_0,
//                        char* ciphertext_1)
//{
//    //char mingwen[17] = {0};
//
//    unsigned char key[17] = "2718281827117589\0";	//密钥 B
//    unsigned char key_1[17] = {0};
//    AES(key);
//    //加密 lisense
//    int len = strlen(lisense);
//    char* mingwen = (char*)malloc(len* sizeof(char));
//    memset((void*)mingwen, 0,len* sizeof(char));
//    if( len == 16)
//    {
//        memcpy( (void *)mingwen, (void *)lisense, len*sizeof(char));
//    }
//    else
//    {
//        LOGI("Lisense format error!");
//        return -1;
//    }
//    Bm53Cipher(mingwen, ciphertext_0);
//
//
//    //用 Android ID 加密 密钥B
//    len = strlen(android_ID);
//    if( len == 16)
//    {
//        memcpy( (void *)key_1, (void *)android_ID, len*sizeof(char));
//        AES(key_1);
//        memset((void*)key_1, 0, sizeof(key_1));		//使用完 密钥，销毁
//        len = strlen(( char *)key);
//        memcpy( (void *)mingwen, (void *)key, len*sizeof(char));
//        Bm53Cipher(mingwen, ciphertext_1);
//    }
//    else
//    {
//        LOGI("android_ID format error!");
//        return -2;
//    }
//    return 0;
//}

void AES::Cipher_input_output(char *input, char *output) {
    unsigned char uch_input[1024];
    strToUChar(input, uch_input);
    Cipher_uchar(uch_input);
    ucharToHex(uch_input, output);
}

/*********************************************
* 函数名：  AES::Bm53Cipher
* 描述：    整段文档加密
* @参数：   char *input
* @参数：   char *output
* @返回值： void
*********************************************/
void AES::Bm53Cipher(char *input, char *output) {
    int nLen = strlen(input);
    char *newAlock = (char *) malloc((strlen(input) + (16 - nLen % 16) + 1) * sizeof(char));
    memcpy((void *) newAlock, (void *) input, nLen * sizeof(char));
    int n = 0;
    for (n = 0; n < (16 - nLen % 16); n++) {
        newAlock[nLen + n] = (16 - nLen % 16);
    }
    newAlock[nLen + (16 - nLen % 16)] = 0;
    int i = 0, j = 0;
    char block[17] = {0}, e_block[33] = {0};    //block[16]
    while (newAlock[i] != '\0') {
        memcpy((void *) block, (void *) (newAlock + i), 16 * sizeof(char));
        Cipher_input_output(block, e_block);
        memcpy((void *) (output + j), (void *) e_block, 32 * sizeof(char));
        for (n = 0; n < 32; n++) {
            *(output + n + j) = e_block[n];        //复制前 16 个字节
        }
        i += 16;
        j += 32;
    }
    *(output + j) = '\0';
    free(newAlock);
}

/*********************************************
* 函数名：  AES::Bm53InvCipher
* 描述：    对整段文字解密
* @参数：   char *input 16进制密文
* @参数：   char *output 原文
* @返回值： void
*********************************************/
void AES::Bm53InvCipher(char *input, char *output) {
    unsigned char *uch_input = (unsigned char *) malloc(strlen(input) / 2);
    unsigned char *uch_output = (unsigned char *) malloc(strlen(output));
    hexToUChar(input, uch_input);
    int nBuf = 0;
    unsigned char ublock[16];
    int n = strlen(input);
    while (nBuf < (int) strlen(input) / 2) {
        memcpy((void *) ublock, (void *) (uch_input + nBuf), 16 * sizeof(char));
        InvCipher_uchar(ublock);
        memcpy((void *) (uch_output + nBuf), (void *) ublock, 16 * sizeof(char));
        nBuf += 16;
    }
    uch_output[nBuf] = 0;//封闭字符串
    ucharToStr(uch_output, output);
    //剔除用于补充16字节的字符
    int nLen = strlen(output);
    for (n = nLen - 1; n >= nLen - 16; n--) {
        if (1 <= *(output + n) && 16 >= *(output + n)) {
            *(output + n) = 0;
        }
        else {
            break;
        }
    }
}

unsigned char *AES::Cipher_uchar(unsigned char *input) {
    unsigned char state[4][4];
    int i, r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            state[r][c] = input[c * 4 + r];
        }
    }
    AddRoundKey(state, w[0]);
    for (i = 1; i <= 10; i++) {
        SubBytes(state);
        ShiftRows(state);
        if (i != 10)MixColumns(state);
        AddRoundKey(state, w[i]);
    }

    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            input[c * 4 + r] = state[r][c];
        }
    }
    return input;
}

unsigned char *AES::InvCipher_uchar(unsigned char *input) {
    unsigned char state[4][4];
    int i, r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            state[r][c] = input[c * 4 + r];
        }
    }
    AddRoundKey(state, w[10]);

    for (i = 9; i >= 0; i--) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, w[i]);
        if (i) {
            InvMixColumns(state);
        }
    }
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            input[c * 4 + r] = state[r][c];
        }
    }
    return input;
}

void AES::KeyExpansion(unsigned char *key, unsigned char w[][4][4]) {
    int i, j, r, c;
    unsigned char rc[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            w[0][r][c] = key[r + c * 4];
        }
    }
    for (i = 1; i <= 10; i++) {
        for (j = 0; j < 4; j++) {
            unsigned char t[4];
            for (r = 0; r < 4; r++) {
                t[r] = j ? w[i][r][j - 1] : w[i - 1][r][3];
            }
            if (j == 0) {
                unsigned char temp = t[0];
                for (r = 0; r < 3; r++) {
                    t[r] = Sbox[t[(r + 1) % 4]];
                }
                t[3] = Sbox[temp];
                t[0] ^= rc[i - 1];
            }
            for (r = 0; r < 4; r++) {
                w[i][r][j] = w[i - 1][r][j] ^ t[r];
            }
        }
    }
}

unsigned char AES::FFmul(unsigned char a, unsigned char b) {
    unsigned char bw[4];
    unsigned char res = 0;
    int i;
    bw[0] = b;
    for (i = 1; i < 4; i++) {
        bw[i] = bw[i - 1] << 1;
        if (bw[i - 1] & 0x80) {
            bw[i] ^= 0x1b;
        }
    }
    for (i = 0; i < 4; i++) {
        if ((a >> i) & 0x01) {
            res ^= bw[i];
        }
    }
    return res;
}

void AES::SubBytes(unsigned char state[][4]) {
    int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            state[r][c] = Sbox[state[r][c]];
        }
    }
}

void AES::ShiftRows(unsigned char state[][4]) {
    unsigned char t[4];
    int r, c;
    for (r = 1; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            t[c] = state[r][(c + r) % 4];
        }
        for (c = 0; c < 4; c++) {
            state[r][c] = t[c];
        }
    }
}

void AES::MixColumns(unsigned char state[][4]) {
    unsigned char t[4];
    int r, c;
    for (c = 0; c < 4; c++) {
        for (r = 0; r < 4; r++) {
            t[r] = state[r][c];
        }
        for (r = 0; r < 4; r++) {
            state[r][c] = FFmul(0x02, t[r])
                          ^ FFmul(0x03, t[(r + 1) % 4])
                          ^ FFmul(0x01, t[(r + 2) % 4])
                          ^ FFmul(0x01, t[(r + 3) % 4]);
        }
    }
}

void AES::AddRoundKey(unsigned char state[][4], unsigned char k[][4]) {
    int r, c;
    for (c = 0; c < 4; c++) {
        for (r = 0; r < 4; r++) {
            state[r][c] ^= k[r][c];
        }
    }
}

void AES::InvSubBytes(unsigned char state[][4]) {
    int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            state[r][c] = InvSbox[state[r][c]];
        }
    }
}

void AES::InvShiftRows(unsigned char state[][4]) {
    unsigned char t[4];
    int r, c;
    for (r = 1; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            t[c] = state[r][(c - r + 4) % 4];
        }
        for (c = 0; c < 4; c++) {
            state[r][c] = t[c];
        }
    }
}

void AES::InvMixColumns(unsigned char state[][4]) {
    unsigned char t[4];
    int r, c;
    for (c = 0; c < 4; c++) {
        for (r = 0; r < 4; r++) {
            t[r] = state[r][c];
        }
        for (r = 0; r < 4; r++) {
            state[r][c] = FFmul(0x0e, t[r])
                          ^ FFmul(0x0b, t[(r + 1) % 4])
                          ^ FFmul(0x0d, t[(r + 2) % 4])
                          ^ FFmul(0x09, t[(r + 3) % 4]);
        }
    }
}

int AES::getUCharLen(const unsigned char *uch) {
    int len = 0;
    while (*uch++)
        ++len;
    return len;
}

int AES::ucharToHex(const unsigned char *uch, char *hex) {
    int high, low;
    int tmp = 0;
    if (uch == NULL || hex == NULL) {
        return -1;
    }
    if (getUCharLen(uch) == 0) {
        return -2;
    }
    while (*uch) {
        tmp = (int) *uch;
        high = tmp >> 4;
        low = tmp & 15;
        *hex++ = valueToHexCh(high); //先写高字节
        *hex++ = valueToHexCh(low); //其次写低字节
        uch++;
    }
    *hex = '\0';
    return 0;
}

int AES::hexToUChar(const char *hex, unsigned char *uch) {
    int high, low;
    int tmp = 0;
    if (hex == NULL || uch == NULL) {
        return -1;
    }
    if (strlen(hex) % 2 == 1) {
        return -2;
    }
    while (*hex) {
        high = ascillToValue(*hex);
        if (high < 0) {
            *uch = '\0';
            return -3;
        }
        hex++; //指针移动到下一个字符上
        low = ascillToValue(*hex);
        if (low < 0) {
            *uch = '\0';
            return -3;
        }
        tmp = (high << 4) + low;
        *uch++ = tmp;
        hex++;
    }
    *uch = (int) '\0';
    return 0;
}

int AES::strToUChar(const char *ch, unsigned char *uch) {
    int tmp = 0;
    if (ch == NULL || uch == NULL)
        return -1;

    if (strlen(ch) == 0)
        return -2;

    while (*ch) {
        tmp = (int) *ch;
        *uch++ = tmp;
        ch++;
    }

    *uch = (int) '\0';
    return 0;
}

int AES::ucharToStr(const unsigned char *uch, char *ch) {
    int tmp = 0;
    if (uch == NULL || ch == NULL)
        return -1;
    while (*uch) {
        tmp = (int) *uch;
        *ch++ = (char) tmp;
        uch++;
    }
    *ch = '\0';
    return 0;
}

int AES::ascillToValue(const char ch) {
    int result = 0;
    //获取16进制的高字节位数据
    if (ch >= '0' && ch <= '9') {
        result = (int) (ch - '0');
    }
    else if (ch >= 'a' && ch <= 'z') {
        result = (int) (ch - 'a') + 10;
    }
    else if (ch >= 'A' && ch <= 'Z') {
        result = (int) (ch - 'A') + 10;
    }
    else {
        result = -1;
    }

    return result;
}

char AES::valueToHexCh(const int value) {
    char result = '\0';
    if (value >= 0 && value <= 9) {
        result = (char) (value + 48); //48为ascii编码的‘0’字符编码值
    }

    else if (value >= 10 && value <= 15) {
        result = (char) (value - 10 + 65); //减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
    }
    else { ;
    }

    return result;
}