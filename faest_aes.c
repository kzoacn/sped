#include "faest_aes_128.c"


void aes_prove(const uint8_t* w, const uint8_t* u, uint8_t** V, const uint8_t* in,
               const uint8_t* out, const uint8_t* chall, uint8_t* a_tilde, uint8_t* b_tilde,
               const faest_paramset_t* params) {
    
    aes_prove_128(w, u, V, in, out, chall, a_tilde, b_tilde, params);
}

uint8_t* aes_verify(const uint8_t* d, uint8_t** Q, const uint8_t* chall_2, const uint8_t* chall_3,
                    const uint8_t* a_tilde, const uint8_t* in, const uint8_t* out,
                    const faest_paramset_t* params) {
    
    return aes_verify_128(d, Q, chall_2, chall_3, a_tilde, in, out, params);
}
