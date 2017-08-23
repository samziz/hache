#include <string.h>
#include "buf.h"

static int _get_buf_length(Buf *buf);
static int _get_chars_length(char *c);
static void _left_shift(Buf *b, int n);

/**** BUF API FUNCS ****/

// Add chars to buf at end of array
// Returns length of buffer
void add_to_buf(Buf *b, char *c)
{
	int b_len = _get_buf_length(b);
	int c_len = _get_chars_length(c);

	for (int i = 0; i < c_len; i++)
	{
		(*b).data[b_len + i] = c[i];
	}

	(*b).len = b_len + c_len;
}

// Get n chars from buf
void get_from_buf(Buf *b, char c[], int n)
{
	for (int i = 0; i < n; i++)
	{
		c[i] = (*b).data[i];
	}

	_left_shift(b, n);
}


/**** STATIC FUNCS ****/

// Count num of chars in buf
// Also updates buf.len
static int _get_buf_length(Buf *b)
{
	int len;

	for (int i = 0; 1; i++) {
		if ((*b).data[i] == 0)
		{
			break;
		}

		len += 1;
	}

	(*b).len = len;
	return len;
}

static int _get_chars_length(char *c)
{
	return sizeof(c) / sizeof(c[0]);
}

// Shift all chars in buf n places to the left
static void _left_shift(Buf *b, int n) 
{
	int len = _get_buf_length(b);
	(*b).len = len;

	for (int i = 0; i < len-1; i++)
	{
		(*b).data[i] = (*b).data[i+1];
	}
}
