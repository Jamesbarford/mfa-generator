#ifndef BYTES_TO_B64
#define BYTES_TO_B64

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/buffer.h>

typedef struct b64buf_t {
	uint8_t *buf;
	size_t size;
} b64buf_t;

void tob64(unsigned char *text, b64buf_t *b64buf);	
void free_b64buf(b64buf_t *b64buf);	
void print_b64buf_oneline(b64buf_t *b64buf);
void print_b64buf(b64buf_t *b64buf);

#endif
