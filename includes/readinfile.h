#ifndef READINFILE_H
#define READINFILE_H

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define READ_STDIN 0x1
#define READ_FILE  0x2

typedef struct mmapd_file_t {
	char *text;
	int fd;
	size_t size;
} mmapd_file_t;

typedef void (*Readbuf)(char *buf);

unsigned int get_line_len(mmapd_file_t *mmapd_file);
void read_stdin(Readbuf read_buf);
void read_exec(char *file_name, Readbuf read_buf, int flags);
void read_file(char *file_name, mmapd_file_t *mmapd_file);
void free_file(mmapd_file_t *mmapd_file);

#endif
