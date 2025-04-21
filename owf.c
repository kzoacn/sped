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
#include "time.h"

bool owf(const uint8_t* key, const uint8_t* input, uint8_t* output, int lambda) {
  faest_paramset_t param_temp;
  switch (lambda)
  {
  case 128:
    param_temp = faest_get_paramset(FAEST_128F);
    break;
  case 192:
    param_temp = faest_get_paramset(FAEST_192F);
    break;
  case 256:
    param_temp = faest_get_paramset(FAEST_256F);
    break;
  case 320:
    param_temp = faest_get_paramset(FAEST_320F);
    break;
  case 512:
    param_temp = faest_get_paramset(FAEST_512F);
    break;
  default:
    fprintf(stderr, "lambda = %d not supported.\n", lambda);
    return false;
  }
  const faest_paramset_t paramset =  param_temp;
  const int n = paramset.faest_param.n;
  const int m = paramset.faest_param.m;
  const int w = paramset.faest_param.w;
  const int d = paramset.faest_param.d;
  //const int lambda = paramset.faest_param.lambda;
  const int output_len = (n+7)/8;
  int ret = 0;  

  memset(output, 0, output_len);

  //generate mat H
  uint8_t *buffer=generate_H_mat(n,m,input,lambda);

  uint8_t *e = (uint8_t *)malloc(m);
  generate_e(e,m,w,d,key,lambda);


  // y=H*e
  uint8_t *y = (uint8_t *)malloc(n);
  memset(y,0,n);
  for(int i=0;i<n;i++)
  for(int j=0;j<m;j++){
    y[i] ^= getH(i,j,n,m,buffer) & e[j];
  }
  //pack y into output
  for(int i=0;i<n;i++){
    output[i/8] ^= (y[i] << (i%8));
  }

  free(e);
  free(y);
  free(buffer); 

  return ret == 0;
}//TODO

bool faest_128s_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 128);
}
bool faest_128f_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 128);
}

bool faest_192s_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 192);
}
bool faest_192f_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 192);
}

bool faest_256s_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 256);
}
bool faest_256f_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 256);
}

bool faest_320s_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 320);
}
bool faest_320f_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 320);
}

bool faest_512s_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 512);
}
bool faest_512f_owf(const uint8_t* key, const uint8_t* input, uint8_t* output){
  return owf(key, input, output, 512);
}