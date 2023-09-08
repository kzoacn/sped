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
  const int n = paramset.faest_param.n;
  const int m = paramset.faest_param.m;
  const int w = paramset.faest_param.w;
  const int d = paramset.faest_param.d;
  const int lambda = paramset.faest_param.lambda;
  const int output_len = (n+7)/8;
  int ret = 0; 

  memset(output, 0, output_len);

  //generate mat H
  uint8_t *buffer, **H;
  buffer = (uint8_t *)malloc(n*m);
  H = (uint8_t **)malloc(sizeof(uint8_t*) * n);
  generate_H_mat(buffer,n,m,input,lambda);
  
  for(int i=0;i<n;i++)
    H[i] = buffer + i*m;
  
  for(int i=0;i<n;i++)
  for(int j=0;j<m;j++){
    if(i==j){
      H[i][j]=1;
    }else{
      if(j<n)
        H[i][j]=0;
      else
        H[i][j]=H[i][j]&1;
    }
  }
  uint8_t *e = (uint8_t *)malloc(m);
  generate_e(e,m,w,d,key,lambda);


  // y=H*e
  uint8_t *y = (uint8_t *)malloc(n);
  memset(y,0,n);
  for(int i=0;i<n;i++)
  for(int j=0;j<m;j++){
    y[i] ^= H[i][j] & e[j];
  }
  //pack y into output
  for(int i=0;i<n;i++){
    output[i/8] ^= (y[i] << (i%8));
  }

  free(e);
  free(y);
  free(H);
  free(buffer);

  return ret == 0;
}//TODO