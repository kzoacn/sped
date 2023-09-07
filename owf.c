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

  const faest_paramset_t paramset =  faest_get_paramset(FAEST_128S);
  const int k = paramset.faest_param.k;
  const int m = paramset.faest_param.m;
  const int w = paramset.faest_param.w;
  const int d = paramset.faest_param.d;
  const int lambda = paramset.faest_param.lambda;
  const int output_len = (k+7)/8;
  int ret = 0; 

  memset(output, 0, output_len);

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

  //generate mat H
  uint8_t *buffer, **H;
  buffer = (uint8_t *)malloc(k*m);
  H = (uint8_t **)malloc(sizeof(uint8_t*) * k);
  generate_H_mat(buffer,k,m,input,lambda);
  
  for(int i=0;i<k;i++)
    H[i] = buffer + i*m;
  
  for(int i=0;i<k;i++)
  for(int j=0;j<m;j++){
    if(i==j){
      H[i][j]=1;
    }else{
      if(j<k)
        H[i][j]=0;
      else
        H[i][j]=H[i][j]&1;
    }
  }
  uint8_t *e = (uint8_t *)malloc(m);
  generate_e(e,m,w,d,key,lambda);


  free(H);
  free(buffer);

  return ret == 0;
}//TODO