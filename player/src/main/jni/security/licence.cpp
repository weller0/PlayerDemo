#include "security/licence.h"

Licence::Licence() {
}

Licence::~Licence() {
}

void Licence::getKeyH(const char *in, char *out) {
    AES *aes = new AES();
    aes->encode(in, out);
    delete aes;
}

void Licence::getKeyA(char *out) {
    AES *aes = new AES();
    aes->encode(getPhoneIMEI(), out);
    delete aes;
}

GLboolean Licence::isAllow(const char *str, const char *r1, const char *r2) {
    AES *aes = new AES();
    GLboolean haveLicence = GL_FALSE;
    if(aes->InvCipher_server(str, getPhoneIMEI(), r1, r2) == 0) {
        haveLicence = GL_TRUE;
    }
    delete aes;
    return haveLicence;
}

char *Licence::getPhoneIMEI() {
    char tmp[32];
    char *imei = (char *) malloc(17 * sizeof(char));
    __system_property_get("ro.serialno", imei);
    sprintf(tmp, "%s0000000000000000", imei);
    memcpy((void *) imei, (void *) tmp, 16 * sizeof(char));
    *(imei + 16) = 0;
    LOGD("[Licence:getPhoneIMEI]IMEI:%s", imei);
    return imei;
}