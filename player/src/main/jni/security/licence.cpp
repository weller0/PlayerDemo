#include "security/licence.h"

Licence::Licence() {
//    pAes = new AES();
}

Licence::~Licence() {
//    delete pAes;
}

void Licence::encode(const char *in, char *out) {
    unsigned char *key = (unsigned char *) malloc(17 * sizeof(unsigned char));
    getKey(key);
    encode(key, in, out);
    free(key);
}

void Licence::encode(const unsigned char *key, const char *in, char *out) {
    unsigned char *keyBuffer = new unsigned char[17];
    memset(keyBuffer, '\0', 17);
    memcpy(keyBuffer, key, 16);
    LOGD("[Licence:encode]key=%s", key);
    AES* aes = new AES(keyBuffer, 1);

    int nLength = 17;
    char *inBuffer = new char[nLength];
    memset(inBuffer, '\0', nLength);
    memcpy(inBuffer, in, nLength);
    aes->Cipher(inBuffer, out);

    sprintf(out, "%s45C7A4FDE80383044E8AD790740EDFFD", out);

    LOGD("[Licence:encode]in=%s", in);
    LOGD("[Licence:encode]out=%s", out);

    delete[] inBuffer;
    free(keyBuffer);
}

void Licence::getEncodeA(char *out) {
    encode(getPhoneIMEI(), out);
}

GLboolean Licence::isAllow(const char *licence, const char *r1, const char *r2) {
    GLboolean haveLicence = GL_FALSE;
    // 使用IMEI作为Key
    AES* aes1 = new AES((unsigned char *) getPhoneIMEI(), 1);

    // 解密秘钥B
    int len = strlen(r2) / 2;
    char *miwen1 = (char *) malloc((len + 1) * sizeof(char));
    memset(miwen1, 0, (len + 1));
    memcpy(miwen1, r2, len * sizeof(char));

    len = 17;
    char *mingwen1 = (char *) malloc((len + 1) * sizeof(char));
    memset(mingwen1, 0, (len + 1) * sizeof(char));
    aes1->InvCipher(miwen1, mingwen1);

    // 使用解密的秘钥B作为Key
    AES* aes2 = new AES((unsigned char *) mingwen1, 1);

    // 解密硬件ID
    len = strlen(r1) / 2;
    char *miwen2 = (char *) malloc((len + 1) * sizeof(char));
    memset(miwen2, 0, (len + 1));
    memcpy(miwen2, r1, len * sizeof(char));

    len = 17;
    char *mingwen2 = (char *) malloc((len + 1) * sizeof(char));
    memset(mingwen2, 0, (len + 1) * sizeof(char));
    aes2->InvCipher(miwen2, mingwen2);

    if (memcmp(mingwen2, licence, 16) == 0) {
        haveLicence = GL_TRUE;
    }

    LOGD("[Licence:isAllow]licence=%s", licence);
    LOGD("[Licence:isAllow]miwen1=%s", miwen1);
    LOGD("[Licence:isAllow]miwen2=%s", miwen2);
    LOGD("[Licence:isAllow]mingwen1=%s", mingwen1);
    LOGD("[Licence:isAllow]mingwen2=%s", mingwen2);
    LOGD("[Licence:isAllow]haveLicence=%d", haveLicence);
    free(miwen1);
    free(miwen2);
    free(mingwen1);
    free(mingwen2);
    delete aes1;
    delete aes2;
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