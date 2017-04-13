#ifndef LICENCE_H
#define LICENCE_H

#include <jni.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/system_properties.h>

#include "security/aes.h"
#include "log.h"

class Licence {
public:
    Licence();

    ~Licence();

    void getEncodeA(char *out);

    void encode(const char *in, char *out);

    GLboolean isAllow(const char *str, const char *r1, const char *r2);

private:
    AES *pAes;

    char *getPhoneIMEI();

    int getKey(unsigned char *key);

    void Byte2Hex(const unsigned char *src, int len, char *dest);

    void Hex2Byte(const char *src, int len, unsigned char *dest);

    int Char2Int(char c);
};

#endif //LICENCE_H