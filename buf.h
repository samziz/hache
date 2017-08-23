#ifndef BUF_H
#define BUF_H

#define MAX_BUF_SIZE 8192
#define MAX_MSG_SIZE 2048


// Buf holds data to be written to disk.
// Treat len as a loose approximation:
// it's only written to in functions which
// have already calculated the buffer length.

typedef struct {
	char *data;
	int len;
} Buf;


void add_to_buf(Buf *b, char *c);
void get_from_buf(Buf *b, char c[], int n);

#endif