/*
*  FIPS-180-2 compliant SHA-256 implementation
*
*  Copyright (C) 2006-2010, Brainspark B.V.
*
*  This file is part of PolarSSL (http://www.polarssl.org)
*  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
*
*  All rights reserved.
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License along
*  with this program; if not, write to the Free Software Foundation, Inc.,
*  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/*
*  The SHA-256 Secure Hash Standard was published by NIST in 2002.
*
*  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
*/

#ifndef PBKDF2_SHA256_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned long total[2];	/*!< number of bytes processed  */
	unsigned long state[8];	/*!< intermediate digest state  */
	unsigned char buffer[64];	/*!< data block being processed */

	unsigned char ipad[64];	/*!< HMAC: inner padding        */
	unsigned char opad[64];	/*!< HMAC: outer padding        */
	int is224;		/*!< 0 => SHA-256, else SHA-224 */
} sha2_context;

/*
* SHA-256 context setup
*/
void sha2_starts(sha2_context *ctx, int is224);

/*
* SHA-256 process buffer
*/
void sha2_update(sha2_context *ctx, const unsigned char *input, size_t ilen);

/*
* SHA-256 final digest
*/
void sha2_finish(sha2_context *ctx, unsigned char output[32]);

/*
* output = SHA-256( input buffer )
*/
void sha2(const unsigned char *input, size_t ilen, unsigned char output[32], int is224);

/*
* SHA-256 HMAC context setup
*/
void sha2_hmac_starts(sha2_context *ctx, const unsigned char *key, size_t keylen, int is224);

/*
* SHA-256 HMAC process buffer
*/
void sha2_hmac_update(sha2_context *ctx, const unsigned char *input, size_t ilen);

/*
* SHA-256 HMAC final digest
*/
void sha2_hmac_finish(sha2_context *ctx, unsigned char output[32]);

/*
* SHA-256 HMAC context reset
*/
void sha2_hmac_reset(sha2_context *ctx);

/*
* output = HMAC-SHA-256( hmac key, input buffer )
*/
void sha2_hmac(const unsigned char *key, size_t keylen, const unsigned char *input, size_t ilen, unsigned char output[32], int is224);


void PKCS5_PBKDF2_HMAC(unsigned char *password, size_t plen, unsigned char *salt, size_t slen, const unsigned long iteration_count, const unsigned long key_length, unsigned char *output);

#ifdef __cplusplus
}
#endif

#endif