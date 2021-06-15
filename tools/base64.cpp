#include "base64.h"


//////////////////////////////////////////////////////////////////////////
// constants
static const signed char Base64_Bytes[128] = {
	-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x3E,   -1,   -1,   -1, 0x3F,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,   -1,   -1,   -1, 0x7F,   -1,   -1,
	-1, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 
	0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,   -1,   -1,   -1,   -1,   -1,
	-1, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 
	0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,   -1,   -1,   -1,   -1,   -1
};

static const char Base64_Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char BASE64_PAD_CHAR = '=';
const char BASE64_PAD_BYTE = 0x7F;


const char* Base64Replace(char* data, char a, char b)
{
	// check args
	if (data == NULL || a == '\0' || b == '\0') return data;

	// we are going to modify the characters
	char* src = data;

	// process the buffer in place
	while (*src) {
		if (*src == a) *src = b;
		src++;
	}

	return src;
}


int Base64Decode(const char* base64, int size, unsigned char* data, bool url_safe /* = false */)
{
	// keep a pointer to the buffer
	unsigned char* buffer = data;
	unsigned int data_size = 0;

	// iterate over all characters
	unsigned char codes[4];
	unsigned int code_count = 0;
	while (size--) {
		unsigned char c = *base64++;
		unsigned int n = sizeof(Base64_Bytes) / sizeof(Base64_Bytes[0]);
		if (c >= n) continue;
		if (url_safe) {
			// remap some characters
			if (c == '-') {
				c = '+';
			} else if (c == '_') {
				c = '/';
			}
		}
		signed char code = Base64_Bytes[c];
		if (code >= 0) {
			// valid code
			codes[code_count++] = code;
			if (code_count == 4) {
				// group complete
				if (codes[0] == BASE64_PAD_BYTE || codes[1] == BASE64_PAD_BYTE) {
					return -1;
				}
				if (codes[2] == BASE64_PAD_BYTE) {
					// pad at char 3
					if (codes[3] == BASE64_PAD_BYTE) {
						// double padding
						unsigned int packed = (codes[0]<<2)|(codes[1]>>4);
						buffer[data_size++] = (unsigned char)packed;
					} else {
						// invalid padding
						return -1;
					}
				} else if (codes[3] == BASE64_PAD_BYTE) {
					// single padding
					unsigned int packed = (codes[0]<<10)|(codes[1]<<4)|(codes[2]>>2);
					buffer[data_size++] = (unsigned char)(packed >> 8);
					buffer[data_size++] = (unsigned char)(packed     );
				} else {
					// no padding
					unsigned int packed = (codes[0]<<18)|(codes[1]<<12)|(codes[2]<<6)|codes[3];
					buffer[data_size++] = (unsigned char)(packed >> 16);
					buffer[data_size++] = (unsigned char)(packed >>  8);
					buffer[data_size++] = (unsigned char)(packed      );
				}
				code_count = 0;
			}
		}
	}

	if (code_count) return -1;

	return data_size;
}

int Base64Encode(const unsigned char* data, int size, char* base64, int max_blocks_per_line /* = 0 */, bool url_safe /* = false */)
{
	unsigned int block_count = 0;
	unsigned int           i = 0;

	// keep a pointer to the buffer
	char* buffer = base64;
	unsigned int data_size = 0;

	// encode each byte
	while (size >= 3) {
		// output a block
		*buffer++ = Base64_Chars[ (data[i  ] >> 2) & 0x3F];
		*buffer++ = Base64_Chars[((data[i  ] & 0x03) << 4) | ((data[i+1] >> 4) & 0x0F)];
		*buffer++ = Base64_Chars[((data[i+1] & 0x0F) << 2) | ((data[i+2] >> 6) & 0x03)];
		*buffer++ = Base64_Chars[  data[i+2] & 0x3F];

		size -= 3;
		i += 3;
		if (++block_count == max_blocks_per_line) {
			*buffer++ = '\r';
			*buffer++ = '\n';
			block_count = 0;
		}
	}

	// deal with the tail
	if (size == 2) {
		*buffer++ = Base64_Chars[ (data[i  ] >> 2) & 0x3F];
		*buffer++ = Base64_Chars[((data[i  ] & 0x03) << 4) | ((data[i+1] >> 4) & 0x0F)];
		*buffer++ = Base64_Chars[ (data[i+1] & 0x0F) << 2];
		*buffer++ = BASE64_PAD_CHAR;
	} else if (size == 1) {
		*buffer++ = Base64_Chars[(data[i] >> 2) & 0x3F];
		*buffer++ = Base64_Chars[(data[i] & 0x03) << 4];
		*buffer++ = BASE64_PAD_CHAR;
		*buffer++ = BASE64_PAD_CHAR;
	}

	// update the string size
	*buffer++ = '\0';
	data_size = strlen(buffer);

	// deal with url safe remapping
	if (url_safe) {
		Base64Replace(buffer, '+', '-');
		Base64Replace(buffer, '/', '_');
	}

	return 0;
}

