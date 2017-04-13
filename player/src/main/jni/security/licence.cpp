#include "security/licence.h"

Licence::Licence() {
    pAes = new AES();
}

Licence::~Licence() {
    delete pAes;
}

void Licence::encode(const char *in, char *out) {
    unsigned char *key = (unsigned char *) malloc(17 * sizeof(unsigned char));
    getKey(key);
    LOGD("[Licence]key=%s", key);
    pAes->setKey(key);
    free(key);

    int nLength = sizeof(in);
    int spaceLength = 16 - (nLength % 16);
    unsigned char* pBuffer = new unsigned char[nLength + spaceLength];
    memset(pBuffer, '\0', nLength + spaceLength);
    memcpy(pBuffer, in, nLength);
    pAes->Cipher(pBuffer);

    // 这里需要把得到的字符数组转换成十六进制字符串
    size_t outSize = sizeof(out);
    memset(out, '\0', outSize);
    Byte2Hex(pBuffer, nLength + spaceLength, out);

    LOGD("[Licence]in=%s", in);
    LOGD("[Licence]out=%s", out);

    delete[] pBuffer;
}

void Licence::getEncodeA(char *out) {
    encode(getPhoneIMEI(), out);
}

GLboolean Licence::isAllow(const char *str, const char *r1, const char *r2) {
    GLboolean haveLicence = GL_TRUE;
    return haveLicence;
}

char *Licence::getPhoneIMEI() {
    char tmp[32];
    char *imei = (char *) malloc(17 * sizeof(char));
    __system_property_get("ro.serialno", imei);
    sprintf(tmp, "%s0000000000000000", imei);
    memcpy((void *) imei, (void *) tmp, 16 * sizeof(char));
    *(imei + 16) = 0;
    return imei;
}

int Licence::getKey(unsigned char *key) {
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
        LOGE("[AES:getKey]malloc key_1 error!");
        return -1;
    }
    memset((void *) key_1, 0, 21 * sizeof(unsigned char));
    sprintf((char *) key_1, "%0.16f\n", e);
    *key = *key_1;
    memcpy((void *) (key + 1), (void *) (key_1 + 2), 15 * sizeof(unsigned char));
    *(key + 16) = 0;
    free(key_1);
    return 0;
}

void Licence::Byte2Hex(const unsigned char* src, int len, char* dest) {
    for (int i=0; i<len; ++i) {
        sprintf(dest + i * 2, "%02X", src[i]);
    }
}

void Licence::Hex2Byte(const char* src, int len, unsigned char* dest) {
    int length = len / 2;
    for (int i=0; i<length; ++i) {
        dest[i] = Char2Int(src[i * 2]) * 16 + Char2Int(src[i * 2 + 1]);
    }
}

int Licence::Char2Int(char c) {
    if ('0' <= c && c <= '9') {
        return (c - '0');
    }
    else if ('a' <= c && c<= 'f') {
        return (c - 'a' + 10);
    }
    else if ('A' <= c && c<= 'F') {
        return (c - 'A' + 10);
    }
    return -1;
}