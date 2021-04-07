#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

static void handle_errors() {
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
}

int encrypt(unsigned char *plaintext, unsigned char *pass, unsigned char *ciphertext) {	
	unsigned char iv[EVP_MAX_IV_LENGTH];
	memset(iv, '\0', EVP_MAX_IV_LENGTH);

	int len;
	int plaintext_len = strlen((char *)plaintext);
	int ciphertext_len;
	const EVP_CIPHER *cipher = EVP_aes_256_cbc();
	
	EVP_CIPHER_CTX *ctx;

	if (!(ctx = EVP_CIPHER_CTX_new()))
		handle_errors();

	if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, (unsigned char *)pass, iv))
		handle_errors();

	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handle_errors();
	ciphertext_len = len;

	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		handle_errors();

	ciphertext_len += len;
	ciphertext[ciphertext_len] = '\0';

	EVP_CIPHER_CTX_free(ctx);

	return -1;
}
