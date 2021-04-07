#ifndef BASE64_TO_HEX
#define BASE64_TO_HEX

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

typedef struct hexbuf_t {
	size_t size;
	uint8_t *buf;
} hexbuf_t;

void decodeB64(hexbuf_t *hexbuf, uint8_t *b64, size_t input_len);
void decodeB64_linebreaks(hexbuf_t *hexbuf, uint8_t *b64, size_t input_len);
void decodeB64_from_file(hexbuf_t *hexbuf, char *file_name);
void print_hexbuf(hexbuf_t *hexbuf);
void print_hexbuf_oneline(hexbuf_t *hexbuf);
void free_hex_buf(hexbuf_t *hexbuf);

#endif
