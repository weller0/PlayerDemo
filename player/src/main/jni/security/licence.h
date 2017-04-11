#ifndef LICENCE_H
#define LICENCE_H

#include <jni.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <sys/system_properties.h>

#include "security/aes.h"
#include "log.h"

class Licence {
public:
    GLboolean haveLicence;

    Licence();

    ~Licence();

    void getKeyA(char *out);

    void getKeyH(const char *in, char *out);

    GLboolean isAllow(const char *str, const char *r1, const char *r2);

private:
    char *getPhoneIMEI();
};

#endif //LICENCE_H