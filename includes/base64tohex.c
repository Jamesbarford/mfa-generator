/**
 * Simple base64 to hex converter
 *
 * https://en.wikipedia.org/wiki/Base64
 *
 * char *example = "TWFu";
 * int hex2 = 0;
 *
 * hex2 |= 19 << 18;
 * hex2 |= 22 << 12;
 * hex2 |= 5  << 6;
 * hex2 |= 46 << 0;
 *	
 * printf("0x%X\n", hex2); -> Man
 * 
 *
 * */
#include "base64tohex.h"
#include "readinfile.h"
#include <stdio.h>

static inline size_t get_line_length(uint8_t *text) {
	size_t i = 0;
	int16_t c;

	while (( c = text[i++]) != '\n')
		if (c == EOF)
			return i;

	return i - 1;
}

// hex -> 0x4D 0x61 0x6E
static inline int8_t base64_to_int(char c) {
	switch(c) {
		case 'A': return 0;
		case 'B': return 1;
		case 'C': return 2;
		case 'D': return 3;
		case 'E': return 4;
		case 'F': return 5;
		case 'G': return 6;
		case 'H': return 7;
		case 'I': return 8;
		case 'J': return 9;
		case 'K': return 10;
		case 'L': return 11;
		case 'M': return 12;
		case 'N': return 13;
		case 'O': return 14;
		case 'P': return 15;
		case 'Q': return 16;
		case 'R': return 17;
		case 'S': return 18;
		case 'T': return 19;
		case 'U': return 20;
		case 'V': return 21;
		case 'W': return 22;
		case 'X': return 23;
		case 'Y': return 24;
		case 'Z': return 25;
		case 'a': return 26;
		case 'b': return 27;
		case 'c': return 28;
		case 'd': return 29;
		case 'e': return 30;
		case 'f': return 31;
		case 'g': return 32;
		case 'h': return 33;
		case 'i': return 34;
		case 'j': return 35;
		case 'k': return 36;
		case 'l': return 37;
		case 'm': return 38;
		case 'n': return 39;
		case 'o': return 40;
		case 'p': return 41;
		case 'q': return 42;
		case 'r': return 43;
		case 's': return 44;
		case 't': return 45;
		case 'u': return 46;
		case 'v': return 47;
		case 'w': return 48;
		case 'x': return 49;
		case 'y': return 50;
		case 'z': return 51;
		case '0': return 52;
		case '1': return 53;
		case '2': return 54;
		case '3': return 55;
		case '4': return 56;
		case '5': return 57;
		case '6': return 58;
		case '7': return 59;
		case '8': return 60;
		case '9': return 61;
		case '+': return 62;
		case '/': return 63;
		default:
			return 0;
	}
}

void alloc_hexbuf(hexbuf_t *hexbuf, size_t input_len) {
	size_t output_len = (input_len * 3 / 4);

	hexbuf->size = output_len;

	if ((hexbuf->buf = malloc(output_len + sizeof(size_t))) == NULL) {
		fprintf(stderr, "Failed to allocate memory for decoded output: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

}

void free_hex_buf(hexbuf_t *hexbuf) {
	free(hexbuf->buf);
	hexbuf->size = 0;
}

static inline void decode(uint8_t *b64, uint8_t *outbuf, size_t in_len, size_t output_len, size_t *current_count) {
	int hex = 0;
	size_t i = 0;

	while (i < in_len) {	
		uint8_t c_1 = b64[i++];
		uint8_t c_2 = b64[i++];
		uint8_t c_3 = b64[i++];
		uint8_t c_4 = b64[i++];

		hex |= base64_to_int(c_1) << 18;
		hex |= base64_to_int(c_2) << 12;
		hex |= base64_to_int(c_3) << 6;	
		hex |= base64_to_int(c_4);

		uint8_t a_1 = (hex >> 16) & 0xFF;
		uint8_t a_2 = (hex >> 8) & 0xFF;
		uint8_t a_3 = hex & 0xFF;

		if (*current_count < output_len) outbuf[(*current_count)++] = a_1;
		if (*current_count < output_len) outbuf[(*current_count)++] = a_2;
		if (*current_count < output_len) outbuf[(*current_count)++] = a_3;

		hex ^= hex;
	}
}

/* This can handle bpth line breaks and no line breaks, if no line breaks it will do a pass over the whole file */
void decodeB64_linebreaks(hexbuf_t *hexbuf, uint8_t *b64, size_t input_len) {
	size_t line_len = get_line_length(b64);
	alloc_hexbuf(hexbuf, input_len);
	
	uint8_t tmp[line_len];
	memset(tmp, '\0', line_len);
	size_t i = 0;
	size_t j = 0;
	size_t cur = 0;
	size_t line_count = 0;

	while (i < input_len + line_len) {
		if (cur != 0 && cur % line_len  == 0) {
			line_count++;
			
			decode(tmp, hexbuf->buf, line_len, hexbuf->size, &j);
			memset(tmp, '\0', line_len);
			cur ^= cur;
			i++;
		} else {
			char c = b64[i++];

			if (c == '=') {
				cur++;
				hexbuf->size--;
			} else {
				 tmp[cur++] = c;
			}
		}
	}

	if (line_count > 1)
		hexbuf->size -= line_count / 4 * 3;
	// TODO: re-alloc to correct size;
	hexbuf->buf[i] = '\0';
	hexbuf->size = strlen((char *)hexbuf->buf);
}

/* This does not handle line breaks, feels more robust */
void decodeB64(hexbuf_t *hexbuf, uint8_t *b64, size_t input_len) {
	size_t j = 0;
	alloc_hexbuf(hexbuf, input_len);

	decode(b64, hexbuf->buf, input_len, hexbuf->size, &j);
	hexbuf->size = strlen((char *)hexbuf->buf);
}

/* the buffer can have nulls in it, making debugging hard */
void decodeB64_from_file(hexbuf_t *hexbuf, char *file_name) {
	mmapd_file_t mmapd_file;

	read_file(file_name, &mmapd_file);
	decodeB64_linebreaks(hexbuf, (uint8_t *)mmapd_file.text, mmapd_file.size);

	free_file(&mmapd_file);
}

void print_hexbuf_oneline(hexbuf_t *hexbuf) {
	for (size_t i = 0; i < hexbuf->size; ++i) {
		printf("%02x", hexbuf->buf[i]);
	}
	printf("\n");
}

// somewhat traditional, could print by bytes
void print_hexbuf(hexbuf_t *hexbuf) {
	for (size_t i = 0; i < hexbuf->size; ++i) {
		if (i != 0 && i % 30 == 0)
			printf("\n");
		printf("%02x", hexbuf->buf[i]);
	}
	printf("\n");
}
