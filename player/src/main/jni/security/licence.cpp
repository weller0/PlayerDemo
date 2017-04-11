#include "security/licence.h"

Licence::Licence() {
    haveLicence = true;
}

Licence::~Licence() {
}

void Licence::getKeyH(const char *in, char *out) {
    AES *aes = new AES();
    aes->encode(getPhoneIMEI(), out);
    delete aes;
}

void Licence::getKeyA(char *out) {
    AES *aes = new AES();
    aes->encode(getPhoneIMEI(), out);
    delete aes;
}

GLboolean Licence::isAllow(const char *str, const char *r1, const char *r2) {
    AES *aes = new AES();
    if (aes->InvCipher_server(str, getPhoneIMEI(), r1, r2) == 0) {
        haveLicence = JNI_TRUE;
    } else {
        haveLicence = JNI_FALSE;
    }
    return haveLicence;
}

char *Licence::getPhoneIMEI() {
    char *imei = (char *) malloc(16 * sizeof(char));
    __system_property_get("ro.serialno", imei);
    return imei;
}