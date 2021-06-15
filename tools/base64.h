#ifndef __BASE64_H__
#define __BASE64_H__


#include <stdio.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////
// constants
const unsigned int BASE64_MIME_BLOCKS_PER_LINE = 19;
const unsigned int BASE64_PEM_BLOCKS_PER_LINE  = 16;


#ifdef __cplusplus
extern "C" 
{
#endif


int Base64Decode(const char* base64, int size, unsigned char* data, bool url_safe = false);
int Base64Encode(const unsigned char* data, int size, char* base64, int max_blocks_per_line = 0, bool url_safe = false);


#ifdef __cplusplus
}
#endif


#endif	// __BASE64_H__