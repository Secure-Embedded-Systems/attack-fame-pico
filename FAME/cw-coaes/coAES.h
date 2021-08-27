#ifndef _COAES_H_
#define _COAES_H_


#define AES_BASE_ADDR 0x80013000
//static volatile int *aes_ptr;
volatile int * volatile aes_ptr;

void coAES_ecb_enc(int* key_enc, volatile int* volatile data_enc, volatile int* volatile ct);
void coAES_ecb_dec(int* key_dec, volatile int* volatile data_dec, volatile int* volatile pt);
void coAES_cbc_enc(int* key_enc, volatile int* volatile data_enc, volatile int* volatile ct, volatile int* volatile init_vector, int byte_n );
void coAES_cbc_dec(int* key_dec, volatile int* volatile data_dec, volatile int* volatile pt, volatile int* volatile init_vector, int byte_n );
#endif
