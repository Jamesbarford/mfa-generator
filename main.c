#include <termios.h>
#include <time.h>
#include <liboath/oath.h>

#include "./includes/base64tohex.h"
#include "./includes/bytestobase64.h"
#include "./includes/readinfile.h"
#include "./decrypt.h"
#include "./encrypt.h"

static int oath_errno;

#define ECHO_ON  0
#define ECHO_OFF 1

static const char *token_file = "tokens.enc";

enum CmdOpts {
	GET = 0x1,
	SET = 0x2,
	DELETE = 0x4
};

enum FileParseMode {
	KEY,
	TOKEN,
	WALK
};

enum NumericBase {
	B64 = 0x1,
	B32 = 0x2,
	B16 = 0x4
};

typedef struct cmd_opts_t {
	int flag;
	int base;
	unsigned char *key;
} cmd_opts_t;

static void invalid_cmdopts() {
	fprintf(stderr, "Can only set one command of the following line options -s, -g, -d\n");
	exit(EXIT_FAILURE);
}

static void print_usage(char *progname) {
	printf(
			"Usage: %s [OPTION] \n\n"
			"  -g <key_name>\n"
			"         get mfa token for key\n"
			"  -s <key_name>\n"
			"         set secret key to generate mfa tokens\n"
			"  -d <key_name>\n"
			"         delete secret key\n\n"
			"  -h\n"
			"         help\n", progname);
}

// Get command line options and validate.
static void get_cmd_opts(cmd_opts_t *cmd_opts, int argc, char **argv) {
	char c;
	cmd_opts->flag = 0;

	while ((c = getopt(argc, argv, "s:g:d")) != -1) {
		switch (c) {
			case 'd':
				if (cmd_opts->flag != 0) invalid_cmdopts();
				cmd_opts->flag = DELETE;
				cmd_opts->key = (unsigned char *)optarg;
				break;
			case 's':
				if (cmd_opts->flag != 0) invalid_cmdopts();
				cmd_opts->flag = SET;
				cmd_opts->key = (unsigned char *)optarg;
				break;
			case 'g':
				if (cmd_opts->flag != 0) invalid_cmdopts();
				cmd_opts->flag = GET;
				cmd_opts->key = (unsigned char *)optarg;
				break;
			default:
				break;
		}
	}

	if (cmd_opts->flag == 0) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

// Seems reasonable for this to error out, otherwise a key will be printed to the terminal -> insecure
void toggle_echo(struct termios *term, int toggle) {
	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "Invalid terminal emulation.\n");
		exit(EXIT_FAILURE);
	}

	if (tcgetattr(STDIN_FILENO, term) == -1) {
		fprintf(stderr, "Failed to get terminal attributes: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (toggle == ECHO_OFF) {
		term->c_lflag &= ~(ICANON | ECHO);
	} else {
		term->c_lflag |= (ICANON | ECHO);
	}

	if (tcsetattr(STDIN_FILENO, TCSANOW, term) == -1) {
		fprintf(stderr, "Failed to set terminal attributes: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void scanf_no_echo(struct termios *term, unsigned char *buf) {
	toggle_echo(term, ECHO_OFF);

	if (scanf("%s", buf) == EOF) {
		fprintf(stderr, "Failed to read from stdin: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	toggle_echo(term, ECHO_ON);
}

// https://www.nongnu.org/oath-toolkit/liboath-api/liboath-oath.h.html#oath-totp-generate
void print_mfa_token(char *secret_key) {
	if ((oath_errno = oath_init()) != OATH_OK) {
		fprintf(stderr, "Failed to initilize oath: %s\n", oath_strerror(oath_errno));
		exit(EXIT_FAILURE);
	}

	size_t len = strlen(secret_key);
	time_t now = time(NULL);
	time_t start_offset = 0;
	unsigned int digits = 6;
	char outbuf[digits + 1];

	if ((oath_errno = oath_totp_generate(secret_key, len, now, OATH_TOTP_DEFAULT_TIME_STEP_SIZE,
					start_offset, digits, outbuf)) != OATH_OK) {
		fprintf(stderr, "Failed to generate MFA token: %s\n", oath_strerror(oath_errno));
		exit(EXIT_FAILURE);
	} 

	printf("%s", outbuf);
	(void)oath_done();
}

void decrypt_file(unsigned char **decrypted_file, unsigned char *pass) {	
	hexbuf_t hexbuf;

	decodeB64_from_file(&hexbuf, (char *)token_file);
	decrypt(&hexbuf, pass, decrypted_file);

	free_hex_buf(&hexbuf);
}

void get_value(cmd_opts_t *cmd_opts, char *token) {
	// token: <- white space before arrow.
	const int TOKEN_PREFIX_SIZE = 7;	
	unsigned char *decrypted_file;
	unsigned char pass[BUFSIZ];
	struct termios term;

	printf("Password: ");
	scanf_no_echo(&term, pass);
	printf("\n");

	decrypt_file(&decrypted_file, pass);

	enum FileParseMode mode = WALK;
	size_t file_size = strlen((char *)decrypted_file);
	bool match = false;
	char current_line[file_size];
	char c;
	int ptr = 0;
	
	memset(current_line, '\0', file_size);

	for (size_t i = 0; i < file_size; ++i) {
		c = decrypted_file[i];

		if (c != '\n') {
			current_line[ptr++] = c;
		} else {
			if (mode == TOKEN) {
				break;
			}
			if (strstr(current_line, (char *)cmd_opts->key) != NULL) {
				match = true;
				memset(current_line, '\0', file_size);
				ptr = 0;
				mode = TOKEN;
				continue;
			}
			memset(current_line, '\0', file_size);
			ptr = 0;
		}
	}

	if (match == true) {
		memcpy(token, current_line + TOKEN_PREFIX_SIZE,
				strlen(current_line) - TOKEN_PREFIX_SIZE);
	} else {
		fprintf(stderr, "Invalid key name\n");
		exit(EXIT_FAILURE);
	}

	free(decrypted_file);
}

void write_to_existing(char *plaintext, unsigned char *pass) {
	unsigned char *decrypted_file;
	unsigned char ciphertext[BUFSIZ];
	int fd;
	b64buf_t b64buf;

	decrypt_file(&decrypted_file, pass);
	strcat(plaintext, (char *)decrypted_file);
	
	encrypt((unsigned char *)plaintext, pass, ciphertext);
	tob64(ciphertext, &b64buf);

	if ((fd = open(token_file, O_CREAT | O_RDWR | O_TRUNC)) <= 0) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (write(fd, b64buf.buf, b64buf.size) != (ssize_t)b64buf.size) {
		fprintf(stderr, "Failed to write key to file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	free(decrypted_file);
	free_b64buf(&b64buf);
	(void)close(fd);
}

void write_to_new(char *plaintext, unsigned char *pass) {
	int fd;
	b64buf_t b64buf;
	unsigned char ciphertext[BUFSIZ];
	
	if ((fd = open(token_file, O_CREAT | O_RDWR | O_APPEND)) <= 0) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fchmod(fd, S_IRUSR | S_IROTH | S_IWUSR | S_IWRITE) == -1) {
		fprintf(stderr, "Failed to chmod '%s': %s\n", token_file, strerror(errno));
		exit(EXIT_FAILURE);
	}

	encrypt((unsigned char *)plaintext, pass, ciphertext);
	tob64(ciphertext, &b64buf);

	if (write(fd, b64buf.buf, b64buf.size) != (ssize_t)b64buf.size) {
		fprintf(stderr, "Failed to write key to file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	free_b64buf(&b64buf);
}

void set_token(cmd_opts_t *cmd_opts) {
	struct termios term;

	unsigned char pass[BUFSIZ];
	unsigned char secret_key[BUFSIZ];
	unsigned char ciphertext[BUFSIZ];
	char plaintext[BUFSIZ];
	
	memset(pass, '\0', BUFSIZ);
	memset(secret_key, '\0', BUFSIZ);
	memset(ciphertext, '\0', BUFSIZ);
	memset(plaintext, '\0', BUFSIZ);

	printf("Secret key to store: ");
	scanf_no_echo(&term, secret_key);
	printf("\n");

	printf("Password: ");
	scanf_no_echo(&term, pass);
	printf("\n");

	int file_exists = access(token_file, F_OK);

	strcat(plaintext, "key: ");
	strcat(plaintext, (char *)cmd_opts->key);
	strcat(plaintext, "\ntoken: ");
	strcat(plaintext, (char *)secret_key);
	strcat(plaintext, "\n\n");

	if (file_exists == 0) {
		write_to_existing(plaintext, pass);
	} else {
		write_to_new(plaintext, pass);
	}
}

int main(int argc, char **argv) {
	cmd_opts_t cmd_opts;
	get_cmd_opts(&cmd_opts, argc, argv);

	switch (cmd_opts.flag) {
		case SET: {
			set_token(&cmd_opts);
			break;
		}
		case GET: {
			char token[BUFSIZ];
			get_value(&cmd_opts, token);
			print_mfa_token(token);
			break;
		}
		case DELETE: {
			fprintf(stderr, "Unimplemented method: DELETE\n");
			exit(EXIT_FAILURE);			
		}
		default:
			break;
	}

	printf("\n");

	exit(EXIT_SUCCESS);
}
