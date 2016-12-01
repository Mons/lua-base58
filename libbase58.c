/*
	base58 lua module
	compile with:
		gcc -Wall -shared -fPIC -o libbase58.so base58.c
		gcc -Wall -shared -fPIC -o libbase58.dylib base58.c
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>


char *nb58 = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";
/****************************************************************************************************************************************************
 * Following code was generated with:                                                                                                                                  *
 * perl -E 'my $bb = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ"; for (0..255) { $i = index $bb, chr($_); print("$i, "); } say ""' *
 ***************************************************************************************************************************************************/
char b58n[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
	-1, 34, 35, 36, 37, 38, 39, 40, 41, -1, 42, 43, 44, 45, 46, -1,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1,
	-1,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -1, 20, 21, 22,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/*
int
encode_base58(unsigned char *input, size_t len, unsigned char **rv, size_t * rvlen) {
	if(len < 1) {
		// printf("ret emp\n");
		*rv = 0;//(unsigned char *)"";
		*rvlen = 0;
		return 0;
	}
	size_t rlen = (len / 2 + 1) * 3;
	
	//printf("Got string of length %d. rlen = %d\n",len, rlen);
	
	unsigned char *ret = malloc(rlen + len);
	
	if (!ret) {
		errno = ENOMEM;
		return 1;
	}
	
	unsigned char *rptr, *src, *ptr, *e;

	rptr = src = ret + rlen;
	
	memcpy(src, input, len);

	e = src + len - 1;

	unsigned char rest;
	unsigned int c;
	
	while(src <= e ) {
		if(*src) {
			// printf("\tnonzero: %d\n",*src);
			rest = 0;
			ptr = src;
			while(ptr <= e) {
				c = rest * 256;
				rest = (c + *ptr) % 58; // (rest * 256 + *ptr) % 58
				*ptr = (c + *ptr) / 58; // (rest * 256 + *ptr) % 58
				// printf("\t%d / %d\n", rest, *ptr);
				ptr++;
			}
			--rptr; *rptr = nb58[rest];
		}
		else {
			// printf("\tzero (%d)\n", e-src );
			src++;
		}
	}

	*rvlen = ret + rlen - rptr;
	if (rptr != ret) {
		memmove(ret,rptr, *rvlen);
	}
	if (*rvlen == 0) {
		// printf("ret 0\n");
		*rvlen = 1;
		ret[0] = nb58[0];
	}
	*rv = ret;
	return 0;
}

int
decode_base58_a(unsigned char *src, size_t len, unsigned char **rv, size_t * rvlen) {
	if(len < 1) {
		*rv    = 0;
		*rvlen = 0;
		return   0;
	}
	size_t i, rlen = (len / 4 + 1) * 3;
	// size_t rlen = len * 2;
	printf("--- start decode rlen = %d (from %d) ---\n", rlen, len);
	unsigned char *ret = malloc(rlen);
	// memset(ret,0,rlen);
	ret[rlen-1] = 0;
	if (!ret) {
		errno = ENOMEM;
		return 1;
	}
	unsigned char *rptr = ret + rlen;
	unsigned int over = 0;

	for (i = 0; i < len; i++) {
		if(b58n[src[i]] < 0) {
			free(ret);
			errno = EINVAL;
			return errno;
		}
		over = over * 58 + b58n[src[i]];
		if (over < 255) continue;
		printf("store %d = '%c' to %p\n", over % 256, over % 256, rptr);
		*rptr-- = over % 256;
		if (rptr < ret) {
			free(ret);
			errno = EFAULT;
			return errno;
		}
		over = over / 256;
	}
	if (over || rptr == ret + rlen) {
		printf("store %d = '%c' to %p\n", over % 256, over % 256, rptr);
		*rptr-- = over;
		if (rptr < ret) {
			free(ret);
			errno = EFAULT;
			return errno;
		}
	}
	*rvlen = ret + rlen - rptr;
	rptr++;
	if (rptr != ret) {
		// printf("move %p -> %p\n", rptr, ret);
		memmove(ret, rptr, *rvlen);
	}
	*rv = ret;
	return 0;
}

int
decode_base58(unsigned char *src, size_t len, unsigned char **rv, size_t * rvlen) {
	if(len < 1) {
		*rv    = 0;
		*rvlen = 0;
		return   0;
	}
	// size_t rlen = (len / 4 + 1) * 3;
	size_t rlen = len * 2;
	printf("--- start decode rlen = %d (from %d) ---\n", rlen, len);
	unsigned char *ret = malloc(rlen);
	memset(ret,0,rlen);
	// ret[rlen-1] = 0;
	if (!ret) {
		errno = ENOMEM;
		return 1;
	}
	unsigned char *rptr = ret + rlen;
	size_t i;
	unsigned int c;
	unsigned char *ptr;
	char rest;
	for(i = 0; i < len; i++) {
		rest = b58n[src[i]];
		printf("rest = %d (%c)\n",rest, src[i]);
		if(rest < 0) {
			free(ret);
			errno = EINVAL;
			return errno;
		}
		printf("%p .. %p\n",ret + rlen - 1, rptr);
		for(ptr = ret + rlen - 1; ptr >= rptr; ptr--) {
			c = rest + *ptr * 58;
			printf("c = %d\n", c);
			*ptr = c % 256;
			rest = c / 256;
		}
		if(rest > 0) {
			rptr--;
			if(rptr < ret) {
				printf("Error xx %p < %p\n", rptr, ret);
				// luaL_error(L, "Error in ");
				free(ret);
				errno = EINVAL;
				return errno;
			}
			*rptr = rest;
		}
	}
	
	*rvlen = ret + rlen - rptr;
	if (rptr != ret) {
		memmove(ret, rptr, *rvlen);
	}
	*rv = ret;
	return 0;
}




int main () {
	unsigned char *out = 0;
	size_t i,ol;
	// if (encode_base58((unsigned char*) "09", 2, &out, &ol) == 0) {
	// 	printf("09 -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	// if (encode_base58((unsigned char*) "\0", 1, &out, &ol) == 0) {
	// 	printf("\\0 -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	// if (encode_base58((unsigned char*) "", 0, &out, &ol) == 0) {
	// 	printf("'' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	// if (encode_base58((unsigned char*) "\1\0\2", 3, &out, &ol) == 0) {
	// 	printf("'\\1\\0\\2' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	printf("decode\n");
	// if (decode_base58((unsigned char*) "1", 1, &out, &ol) == 0) {
	// 	printf("'1' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	// if (decode_base58((unsigned char*) "", 0, &out, &ol) == 0) {
	// 	printf("'' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	// if (decode_base58((unsigned char*) "4ER", 3, &out, &ol) == 0) {
	// 	printf("'4ER' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
	if (decode_base58_a((unsigned char*) "nsss", 4, &out, &ol) == 0) {
		printf("'...' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
		for (i=0;i<ol;i++) {
			printf("%02x",out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	if (decode_base58((unsigned char*) "nsss", 4, &out, &ol) == 0) {
		printf("'...' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
		for (i=0;i<ol;i++) {
			printf("%02x",out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	// if (decode_base58((unsigned char*) "sqmB6yyry95JxHpKBQipHQ", 22, &out, &ol) == 0) {
	// 	printf("'...' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
	// } else {
	// 	printf("error %s?\n",strerror(errno));
	// }
}


*/


static const int8_t b58digits_map[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
	-1, 34, 35, 36, 37, 38, 39, 40, 41, -1, 42, 43, 44, 45, 46, -1,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1,
	-1,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -1, 20, 21, 22,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, -1, -1, -1, -1,
};

static const char b58digits_ordered[] = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";

bool encode_base58(const void *data, size_t binsz, char *b58, size_t *b58sz) {
	// printf("encoding %d bytes of data into %d buf\n", binsz, *b58sz);
	const uint8_t *bin = data;
	int carry;
	ssize_t i, j, high, zcount = 0;
	size_t size;
	
	while (zcount < binsz && !bin[zcount])
		++zcount;
	
	size = (binsz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);
	
	if (*b58sz <= zcount + size - j) {
		*b58sz = zcount + size - j + 1;
		errno = ENOMEM;
		// printf("no more space\n");
		return false;
	}
	
	if (zcount)
		memset(b58, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
		b58[i] = b58digits_ordered[buf[j]];
	b58[i] = '\0';
	*b58sz = i;
	
	// printf("success\n");
	return true;
}

bool decode_base58(const char *b58, size_t b58sz, void *bin, size_t *binszp) {
	size_t binsz = *binszp;
	const unsigned char *b58u = (void*)b58;
	unsigned char *binu = bin;
	size_t outisz = (binsz + 3) / 4;
	uint32_t outi[outisz];
	uint64_t t;
	uint32_t c;
	size_t i, j;
	uint8_t bytesleft = binsz % 4;
	uint32_t zeromask = bytesleft ? (0xffffffff << (bytesleft * 8)) : 0;
	unsigned zerocount = 0;
	
	if (!b58sz)
		b58sz = strlen(b58);
	
	memset(outi, 0, outisz * sizeof(*outi));
	
	// Leading zeros, just count
	for (i = 0; i < b58sz && b58u[i] == '1'; ++i)
		++zerocount;
	
	for ( ; i < b58sz; ++i)
	{
		if (b58u[i] & 0x80) {
			// High-bit set on invalid digit
			errno = EINVAL;
			return false;
		}
		if (b58digits_map[b58u[i]] == -1) {
			// Invalid base58 digit
			errno = EINVAL;
			return false;
		}
		c = (unsigned)b58digits_map[b58u[i]];
		for (j = outisz; j--; )
		{
			t = ((uint64_t)outi[j]) * 58 + c;
			c = (t & 0x3f00000000) >> 32;
			outi[j] = t & 0xffffffff;
		}
		if (c) {
			// Output number too big (carry to the next int32)
			errno = EINVAL;
			return false;
		}
		if (outi[0] & zeromask) {
			// Output number too big (last int32 filled too far)
			errno = EINVAL;
			return false;
		}
	}
	
	j = 0;
	switch (bytesleft) {
		case 3:
			*(binu++) = (outi[0] &   0xff0000) >> 16;
		case 2:
			*(binu++) = (outi[0] &     0xff00) >>  8;
		case 1:
			*(binu++) = (outi[0] &       0xff);
			++j;
		default:
			break;
	}
	
	for (; j < outisz; ++j)
	{
		*(binu++) = (outi[j] >> 0x18) & 0xff;
		*(binu++) = (outi[j] >> 0x10) & 0xff;
		*(binu++) = (outi[j] >>    8) & 0xff;
		*(binu++) = (outi[j] >>    0) & 0xff;
	}
	
	// Count canonical base58 byte count
	binu = bin;
	for (i = 0; i < binsz; ++i)
	{
		if (binu[i])
			break;
		--*binszp;
	}
	*binszp += zerocount;
	
	return true;
}

int main () {
	char out[255];
	size_t i,
		osz = sizeof(out),
		ol = sizeof(out)
	;
	if (decode_base58( "4ER", 3, out, &ol)) {
		printf("'4ER' -> '%-.*s' (%zu)\n", (int)ol, out+osz-ol, ol);
		for (i=osz-ol;i<osz;i++) {
			printf("%02x",out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (decode_base58( "nsss", 4, out, &ol)) {
		printf("'nsss' -> '%-.*s' (%zu)\n", (int)ol, out+osz-ol, ol);
		for (i=osz-ol;i<osz;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (decode_base58( "1", 1, out, &ol)) {
		printf("'1' -> '%-.*s' (%zu)\n", (int)ol, out+osz-ol, ol);
		for (i=osz-ol;i<osz;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (decode_base58( "111", 3, out, &ol)) {
		printf("'1' -> '%-.*s' (%zu)\n", (int)ol, out+osz-ol, ol);
		for (i=osz-ol;i<osz;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (encode_base58( "09", 2, out, &ol)) {
		printf("'09' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
		for (i=0;i<ol;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (encode_base58( "\009", 3, out, &ol)) {
		printf("'\\009' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
		for (i=0;i<ol;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}
	ol = osz;
	if (encode_base58( "\0\0\0", 3, out, &ol)) {
		printf("'\\0' -> '%-.*s' (%zu)\n", (int)ol, out, ol);
		for (i=0;i<ol;i++) {
			printf("%02x",(unsigned char)out[i]);
		}
		printf("\n");
	} else {
		printf("error %s?\n",strerror(errno));
	}

}







