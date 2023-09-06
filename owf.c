/*
 *  SPDX-License-Identifier: MIT
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "owf.h"
#include "aes.h"

bool owf_128(const uint8_t* key, const uint8_t* input, uint8_t* output) {
  aes_round_keys_t round_keys;
  int ret = aes128_init_round_keys(&round_keys, key);
  ret |= aes128_encrypt_block(&round_keys, input, output);
  return ret == 0;
}//TODO