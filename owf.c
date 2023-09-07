/*
 *  SPDX-License-Identifier: MIT
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "owf.h"
#include "aes.h"
#include "utils.h"
#include "random_oracle.h"
#include "fields.h"

bool owf_128(const uint8_t* key, const uint8_t* input, uint8_t* output) {
  int ret = 0; 

  for(int i = 0; i < 16; i++) {
    output[i]=input[i]^key[i];
  }
  for(int i=0;i+1<128;i++){
    // i th bit of key
    int bit_a = (key[i/8] >> (i%8)) & 1;
    int bit_b = (key[(i+1)/8] >> ((i+1)%8)) & 1;
    int bit_c = bit_a & bit_b;
    //set i-th bit of output to bit_c
    output[i/8] ^= (bit_c << (i%8));
  }
  // int ret=0;
  // int lambda = 128;




  // {
  //   H_mat_context_t ctx;
  //   H_mat_init(&ctx, lambda);
  //   H_mat_update(&ctx, input, lambda / 8);

  // }
  


  return ret == 0;
}//TODO