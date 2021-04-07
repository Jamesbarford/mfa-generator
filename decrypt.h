#ifndef DECRYPT_H
#define DECRYPT_H

#include "./includes/base64tohex.h"

int decrypt(hexbuf_t *hexbuf, unsigned char *pass, unsigned char **outbuf);

#endif
