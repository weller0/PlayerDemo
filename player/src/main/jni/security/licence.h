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

    void encode(const unsigned char *key, const char *in, char *out);

    GLboolean isAllow(const char *licence, const char *r1, const char *r2);

private:
//    AES *pAes;

    char *getPhoneIMEI();

    int getKey(unsigned char *key);
};

#endif //LICENCE_H