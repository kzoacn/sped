//
//  rng.c
//
//  Created by Bassham, Lawrence E (Fed) on 8/29/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//

#include <string.h>
#include "rng.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

typedef struct {
  unsigned char Key[32];
  unsigned char V[16];
  int reseed_counter;
} AES256_CTR_DRBG_struct;

static AES256_CTR_DRBG_struct DRBG_ctx;

static void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

// Use whatever AES implementation you have. This uses AES from openSSL library
//    key - 256-bit AES key
//    ctr - a 128-bit plaintext value
//    buffer - a 128-bit ciphertext value
void AES256_ECB(unsigned char* key, unsigned char* ctr, unsigned char* buffer) {
  EVP_CIPHER_CTX* ctx;

  int len;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new()))
    handleErrors();

  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL))
    handleErrors();

  if (1 != EVP_EncryptUpdate(ctx, buffer, &len, ctr, 16))
    handleErrors();

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);
}

static void AES256_CTR_DRBG_Update(unsigned char* provided_data, unsigned char* Key,
                                   unsigned char* V) {
  unsigned char temp[48];

  for (int i = 0; i < 3; i++) {
    // increment V
    for (int j = 15; j >= 0; j--) {
      if (V[j] == 0xff)
        V[j] = 0x00;
      else {
        V[j]++;
        break;
      }
    }

    AES256_ECB(Key, V, temp + 16 * i);
  }
  if (provided_data != NULL)
    for (int i = 0; i < 48; i++)
      temp[i] ^= provided_data[i];
  memcpy(Key, temp, 32);
  memcpy(V, temp + 32, 16);
}

void randombytes_init(unsigned char* entropy_input, unsigned char* personalization_string,
                      int security_strength) {
  (void)security_strength;
  unsigned char seed_material[48];

  memcpy(seed_material, entropy_input, 48);
  if (personalization_string)
    for (int i = 0; i < 48; i++)
      seed_material[i] ^= personalization_string[i];
  memset(DRBG_ctx.Key, 0x00, 32);
  memset(DRBG_ctx.V, 0x00, 16);
  AES256_CTR_DRBG_Update(seed_material, DRBG_ctx.Key, DRBG_ctx.V);
  DRBG_ctx.reseed_counter = 1;
}

int randombytes(unsigned char* x, unsigned long long xlen) {
  unsigned char block[16];
  int i = 0;

  while (xlen > 0) {
    // increment V
    for (int j = 15; j >= 0; j--) {
      if (DRBG_ctx.V[j] == 0xff)
        DRBG_ctx.V[j] = 0x00;
      else {
        DRBG_ctx.V[j]++;
        break;
      }
    }
    AES256_ECB(DRBG_ctx.Key, DRBG_ctx.V, block);
    if (xlen > 15) {
      memcpy(x + i, block, 16);
      i += 16;
      xlen -= 16;
    } else {
      memcpy(x + i, block, xlen);
      xlen = 0;
    }
  }
  AES256_CTR_DRBG_Update(NULL, DRBG_ctx.Key, DRBG_ctx.V);
  DRBG_ctx.reseed_counter++;

  return RNG_SUCCESS;
}
