#include "./bytestobase64.h"

void tob64(unsigned char *text, b64buf_t *b64buf) {	
	BIO *bio, *b64;
	BUF_MEM *buffer_ptr;
	
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, text, strlen((char *)text));
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &buffer_ptr);
	BIO_set_close(bio, BIO_NOCLOSE);

	if ((b64buf->buf = (uint8_t *)malloc(sizeof(uint8_t) * buffer_ptr->length)) == NULL) {
		fprintf(stderr, "Failed to allocate memory for base64 buffer: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy((char *)b64buf->buf, buffer_ptr->data);
	b64buf->size = buffer_ptr->length;
	BIO_free_all(bio);
	BUF_MEM_free(buffer_ptr);
}

void free_b64buf(b64buf_t *b64buf) {
	b64buf->size = 0;
	free(b64buf->buf);
}	

void print_b64buf_oneline(b64buf_t *b64buf) {
	for (size_t i = 0; i < b64buf->size; ++i) {
		printf("%c", b64buf->buf[i]);
	}
	printf("\n");
}

void print_b64buf(b64buf_t *b64buf) {
	for (size_t i = 0; i < b64buf->size; ++i) {
		if (i != 0 && i % 30 == 0)
			printf("\n");
		printf("%c", b64buf->buf[i]);
	}
	printf("\n");
}
