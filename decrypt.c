#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "decrypt.h"
#include "./includes/base64tohex.h"

#define FAIL 0

static void handle_errors() {
	ERR_print_errors_fp(stderr);
	abort();
}

int decrypt(hexbuf_t *hexbuf, unsigned char *pass, unsigned char **outbuf) {
	unsigned char iv[EVP_MAX_IV_LENGTH];
	unsigned char plaintext[256];
	int plaintext_len, len;

	memset(iv, '\0', EVP_MAX_IV_LENGTH);
	memset(plaintext, '\0', 256);

	EVP_CIPHER_CTX *cipherctx; 

	if (!(cipherctx = EVP_CIPHER_CTX_new()))
			handle_errors(); 
	
	if (!EVP_DecryptInit_ex(cipherctx, EVP_aes_256_cbc(), NULL, (unsigned char *)pass, iv)) 
			handle_errors(); 
	
	if (!EVP_DecryptUpdate(cipherctx, plaintext, &len, hexbuf->buf, hexbuf->size))
			handle_errors();
	
	plaintext_len = len;

	if (!EVP_DecryptFinal_ex(cipherctx, plaintext + len, &len))
			handle_errors();

	plaintext_len += len; 
	EVP_CIPHER_CTX_free(cipherctx); 
	plaintext[plaintext_len] = '\0';

	if ((*outbuf = (unsigned char *)malloc(sizeof(unsigned char) * plaintext_len)) == NULL) {
		fprintf(stderr, "Failed to allocate memory for paintext: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy((char *)*outbuf, (char *)plaintext);

	return plaintext_len;
}
