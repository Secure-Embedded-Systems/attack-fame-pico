#include "coAES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trigger/FAME_trigger.h"

//int i, read_data_0, read_data_1, read_data_2, read_data_3;

//ECB ENCRYPT
void coAES_ecb_enc(int* key_enc, volatile int* volatile data_enc, volatile int* volatile ct) {
	int i;
    aes_ptr = AES_BASE_ADDR;
	aes_ptr[0] = key_enc[0];	//key_encrypt_0
	aes_ptr[1] = key_enc[1];	//key_encrypt_1
	aes_ptr[2] = key_enc[2];	//key_encrypt_2
	aes_ptr[3] = key_enc[3];	//key_encrypt_3
    for (int i = 0; i<5; i++) {
			aes_ptr[8]  = ((int *) data_enc)[0] ^ aes_ptr[16];
			aes_ptr[9]  = ((int *) data_enc)[1] ^ aes_ptr[17];
			aes_ptr[10] = ((int *) data_enc)[2] ^ aes_ptr[18];
			aes_ptr[11] = ((int *) data_enc)[3] ^ aes_ptr[19];
			aes_ptr[32] = 0;
			aes_ptr[32] = 1;
			((int *) ct) [0] = aes_ptr[24];
			((int *) ct) [1] = aes_ptr[25];
			((int *) ct) [2] = aes_ptr[26];
			((int *) ct) [3] = aes_ptr[27];
			// uncomment for random encryptions
			memcpy(data_enc, ct, 16);
		}


	////// aes_ptr[0] = key_enc[0];	//key_encrypt_0
	////// aes_ptr[1] = key_enc[1];	//key_encrypt_1
	////// aes_ptr[2] = key_enc[2];	//key_encrypt_2
	////// aes_ptr[3] = key_enc[3];	//key_encrypt_3
	
	//aes_ptr[16] = init_vector_enc[0];	//init_vector_encrypt_0
	//aes_ptr[17] = init_vector_enc[1];	//init_vector_encrypt_1
	//aes_ptr[18] = init_vector_enc[2];	//init_vector_encrypt_2
	//aes_ptr[19] = init_vector_enc[3];	//init_vector_encrypt_3
	
    //input data xor with init_vector because we cannot set init_vector to 0
	//////aes_ptr[8] = data_enc[0] ^ aes_ptr[16];	//data_encrypt_0
	//////aes_ptr[9] = data_enc[1] ^ aes_ptr[17];	//data_encrypt_1
	//////aes_ptr[10] = data_enc[2] ^ aes_ptr[18];	//data_encrypt_2
	//////aes_ptr[11] = data_enc[3] ^ aes_ptr[19];	//data_encrypt_3
	
	////// SCA_TRIGGER();
	////// aes_ptr[32] = 0x00000001;	//control_register
	////// //for(i = 0; i < 10; i++); 
	////// aes_ptr[32] = 0x00000000;	//control_register
	
	//for(i = 0; i < 10; i++); 
//	while(aes_ptr[33] != 0x00000001);

	////// ct[0] = aes_ptr[24];
	////// ct[1] = aes_ptr[25];
	////// ct[2] = aes_ptr[26];
	////// ct[3] = aes_ptr[27];

//	ct[0] = aes_ptr[24];
//	ct[1] = aes_ptr[25];
//	ct[2] = aes_ptr[26];
//	ct[3] = aes_ptr[27];
	//ct[3] = aes_ptr[27];
	//ct[2] = aes_ptr[26];
	//ct[1] = aes_ptr[25];
	//ct[0] = aes_ptr[24];

/*	int temp = rand()%1000;
	ct[0] = temp;
	ct[1] = temp-1;
	ct[2] = temp-2;
	ct[3] = temp-3;
*/	
	//printf("AES output = %08x \n", ct[0]);
	//printf("AES output = %08x \n", ct[1]);
	//printf("AES output = %08x \n", ct[2]);
	//printf("AES output = %08x \n", ct[3]);
/*
	aes_ptr[32] = 0x00000001;	//control_register
	//for(i = 0; i < 10; i++); 
	aes_ptr[32] = 0x00000000;	//control_register
	
	//for(i = 0; i < 1000; i++); 
	while(aes_ptr[33] != 0x00000001);

	ct[0] = aes_ptr[24];
	ct[1] = aes_ptr[25];
	ct[2] = aes_ptr[26];
	ct[3] = aes_ptr[27];
*/
	
//	printf("AES output = %08x \n", ct[0]);
//	printf("AES output = %08x \n", ct[1]);
//	printf("AES output = %08x \n", ct[2]);
//	printf("AES output = %08x \n", ct[3]);
}

//ECB DECRYPT
void coAES_ecb_dec(int* key_dec, volatile int* volatile data_dec, volatile int* volatile pt) {
	int i;
	volatile int init_vector_dec[4];
	aes_ptr = AES_BASE_ADDR;

	aes_ptr[4] = key_dec[0];	//key_decrypt_0
	aes_ptr[5] = key_dec[1];	//key_decrypt_1
	aes_ptr[6] = key_dec[2];	//key_decrypt_2
	aes_ptr[7] = key_dec[3];	//key_decrypt_3
    for (int i = 0; i<8; i++) {
    		aes_ptr[12] = data_dec[0];	//data_decrypt_0
			aes_ptr[13] = data_dec[1];	//data_decrypt_1
			aes_ptr[14] = data_dec[2];	//data_decrypt_2
			aes_ptr[15] = data_dec[3];	//data_decrypt_3

			init_vector_dec[0] = aes_ptr[20];	
			init_vector_dec[1] = aes_ptr[21];	
			init_vector_dec[2] = aes_ptr[22];	
			init_vector_dec[3] = aes_ptr[23];	

			//SCA_TRIGGER();
			aes_ptr[32] = 0x00000002;	//control_register
			//for(i = 0; i < 10; i++); 
			aes_ptr[32] = 0x00000000;	//control_register	

			pt[0] = aes_ptr[28] ^ init_vector_dec[0];
			pt[1] = aes_ptr[29] ^ init_vector_dec[1];
			pt[2] = aes_ptr[30] ^ init_vector_dec[2];
			pt[3] = aes_ptr[31] ^ init_vector_dec[3];

			// aes_ptr[8]  = ((int *) data_enc)[0] ^ aes_ptr[16];
			// aes_ptr[9]  = ((int *) data_enc)[1] ^ aes_ptr[17];
			// aes_ptr[10] = ((int *) data_enc)[2] ^ aes_ptr[18];
			// aes_ptr[11] = ((int *) data_enc)[3] ^ aes_ptr[19];
			// aes_ptr[32] = 0;
			// aes_ptr[32] = 1;
			// ((int *) ct) [0] = aes_ptr[24];
			// ((int *) ct) [1] = aes_ptr[25];
			// ((int *) ct) [2] = aes_ptr[26];
			// ((int *) ct) [3] = aes_ptr[27];
	// uncomment for random encryptions
	memcpy(data_dec, pt, 16);
	}

	////// aes_ptr[4] = key_dec[0];	//key_decrypt_0
	////// aes_ptr[5] = key_dec[1];	//key_decrypt_1
	////// aes_ptr[6] = key_dec[2];	//key_decrypt_2
	////// aes_ptr[7] = key_dec[3];	//key_decrypt_3
	
	//////aes_ptr[12] = data_dec[0];	//data_decrypt_0
	//////aes_ptr[13] = data_dec[1];	//data_decrypt_1
	//////aes_ptr[14] = data_dec[2];	//data_decrypt_2
	//////aes_ptr[15] = data_dec[3];	//data_decrypt_3
	
	//aes_ptr[20] = init_vector_dec[0];	//init_vector_decrypt_0
	//aes_ptr[21] = init_vector_dec[1];	//init_vector_decrypt_1
	//aes_ptr[22] = init_vector_dec[2];	//init_vector_decrypt_2
	//aes_ptr[23] = init_vector_dec[3];	//init_vector_decrypt_3

	//////init_vector_dec[0] = aes_ptr[20];	
	//////init_vector_dec[1] = aes_ptr[21];	
	//////init_vector_dec[2] = aes_ptr[22];	
	//////init_vector_dec[3] = aes_ptr[23];	
	
//	printf("ivd=%08x\n", aes_ptr[20]);	
//	printf("ivd=%08x\n", aes_ptr[21]);	
//	printf("ivd=%08x\n", aes_ptr[22]);	
//	printf("ivd=%08x\n", aes_ptr[23]);	
	
	//SCA_TRIGGER();
	//////aes_ptr[32] = 0x00000002;	//control_register
	//for(i = 0; i < 10; i++); 
	//////aes_ptr[32] = 0x00000000;	//control_register	
	
	//////for(i = 0; i < 10000; i++); 
//	printf("1\n");
//	while(aes_ptr[33] != 0x00000002);
//	printf("2\n");
	
	//////pt[0] = aes_ptr[28] ^ init_vector_dec[0];
	//////pt[1] = aes_ptr[29] ^ init_vector_dec[1];
	//////pt[2] = aes_ptr[30] ^ init_vector_dec[2];
	//////pt[3] = aes_ptr[31] ^ init_vector_dec[3];
//	pt[0] = aes_ptr[28] ^ init_vector_dec[0];
//	pt[1] = aes_ptr[29] ^ init_vector_dec[1];
//	pt[2] = aes_ptr[30] ^ init_vector_dec[2];
//	pt[3] = aes_ptr[31] ^ init_vector_dec[3];

/*	int temp = rand()%1000;
	pt[0] = temp;	
	pt[1] = temp-1;	
	pt[2] = temp-2;	
	pt[3] = temp-3;	
*/
//	printf("op2=%08x\n", aes_ptr[28]);	
//	printf("op2=%08x\n", aes_ptr[29]);	
//	printf("op2=%08x\n", aes_ptr[30]);	
//	printf("op2=%08x\n", aes_ptr[31]);	

//	printf("AES output = %08x \n", pt[0]);
//	printf("AES output = %08x \n", pt[1]);
//	printf("AES output = %08x \n", pt[2]);
//	printf("AES output = %08x \n", pt[3]);

/*
	aes_ptr[32] = 0x00000002;	//control_register
	//for(i = 0; i < 10; i++); 
	aes_ptr[32] = 0x00000000;	//control_register
	
	//for(i = 0; i < 1000; i++); 
	while(aes_ptr[33] != 0x00000002);

	pt[0] = aes_ptr[28];
	pt[1] = aes_ptr[29];
	pt[2] = aes_ptr[30];
	pt[3] = aes_ptr[31];
*/
	
//	printf("AES output = %08x \n", pt[0]);
//	printf("AES output = %08x \n", pt[1]);
//	printf("AES output = %08x \n", pt[2]);
//	printf("AES output = %08x \n", pt[3]);

	/*
	read_data_0 = aes_ptr[20];
	read_data_1 = aes_ptr[21];
	read_data_2 = aes_ptr[22];
	read_data_3 = aes_ptr[23];

	printf("AES output = %08x \n", read_data_0);
	printf("AES output = %08x \n", read_data_1);
	printf("AES output = %08x \n", read_data_2);
	printf("AES output = %08x \n", read_data_3);
	*/
}


void coAES_cbc_enc(int* key_enc, volatile int* volatile data_enc, volatile int* volatile ct, volatile int* volatile init_vector, int byte_n ) {
	int i;
    aes_ptr = AES_BASE_ADDR;
    
    //Set correct IV
    int PT[4];
    int IV[4];
    //ECB deryption ECB_dec(IV, Key) -> PT
    coAES_ecb_dec(key_enc, init_vector, PT);
    //ECB encryption ECB_enc(PT, Key) -> IV
    coAES_ecb_enc(key_enc, PT, IV);
    
    //Perform real AES
	aes_ptr[0] = key_enc[0];	//key_encrypt_0
	aes_ptr[1] = key_enc[1];	//key_encrypt_1
	aes_ptr[2] = key_enc[2];	//key_encrypt_2
	aes_ptr[3] = key_enc[3];	//key_encrypt_3
    
    int block_n = byte_n/16;
    if (byte_n%16 != 0) block_n++;
    for (i=0; i<block_n; i++) {
        aes_ptr[8] = data_enc[4*i];	//data_encrypt_0
        aes_ptr[9] = data_enc[4*i+1];	//data_encrypt_1
        aes_ptr[10] = data_enc[4*i+2];	//data_encrypt_2
        aes_ptr[11] = data_enc[4*i+3];	//data_encrypt_3
        
        aes_ptr[32] = 0x00000001;	//control_register
        aes_ptr[32] = 0x00000000;	//control_register
        for(i = 0; i < 10000; i++); 
    //	while(aes_ptr[33] != 0x00000001);
        ct[4*i] = aes_ptr[24];
        ct[4*i+1] = aes_ptr[25];
        ct[4*i+2] = aes_ptr[26];
        ct[4*i+3] = aes_ptr[27];
    }
}

void coAES_cbc_dec(int* key_dec, volatile int* volatile data_dec, volatile int* volatile pt, volatile int* volatile init_vector, int byte_n ) {
	int i;
    aes_ptr = AES_BASE_ADDR;

    //first run one dummy decryption with IV as input
	aes_ptr[4] = key_dec[0];	//key_decrypt_0
	aes_ptr[5] = key_dec[1];	//key_decrypt_1
	aes_ptr[6] = key_dec[2];	//key_decrypt_2
	aes_ptr[7] = key_dec[3];	//key_decrypt_3
	
	aes_ptr[12] = init_vector[0];	//data_decrypt_0
	aes_ptr[13] = init_vector[1];	//data_decrypt_1
	aes_ptr[14] = init_vector[2];	//data_decrypt_2
	aes_ptr[15] = init_vector[3];	//data_decrypt_3

    int block_n = byte_n/16;
    if (byte_n%16 != 0) block_n++;
    for (i=0; i<block_n; i++) {
        aes_ptr[12] = data_dec[4*i];	//data_decrypt_0
        aes_ptr[13] = data_dec[4*i+1];	//data_decrypt_1
        aes_ptr[14] = data_dec[4*i+2];	//data_decrypt_2
        aes_ptr[15] = data_dec[4*i+3];	//data_decrypt_3
        
        aes_ptr[32] = 0x00000002;	//control_register
        aes_ptr[32] = 0x00000000;	//control_register	
        for(i = 0; i < 10000; i++); 
    //	while(aes_ptr[33] != 0x00000002);

        pt[4*i] = aes_ptr[28];
        pt[4*i+1] = aes_ptr[29];
        pt[4*i+2] = aes_ptr[30];
        pt[4*i+3] = aes_ptr[31];

    }
}

int main(){

	int input[16] = { 0xcafebabe, 0xdeadbeef, 0xdeadbeef, 0x65236723 };
 	int key[16] = { 0x54eeff11, 0x2132eb43, 0xbbaaff23, 0x98541212};
	int output[16];
	init_gpio(); //for trigge

	while(1) {
	// for (int i = 0; i < 4; i++){
	// 	input[i] = 0x01 + input[i];
	SCA_TRIGGER();
 	//coAES_ecb_enc(key,input,output);
 	coAES_ecb_dec(key,input,output);
	}


//   //coaes  encryption
//   printf("COAES ECB ENCRYPTION\n");
//   int key[16] = { 0x54eeff11, 0x2132eb43, 0xbbaaff23, 0x98541212};
//   printf("128-bit key: ");
//   for (int i = 0; i < 4; i++){
//   printf("%x", key[i]);
//   } 

//   int input[16] = { 0xcafebabe, 0xdeadbeef, 0xdeadbeef, 0x65236723 };
//   printf("\n128-bit input: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", input[i]);
//   }

//   int output[16];
//   coAES_ecb_enc(key,input,output);
//   printf("\n128-bit output: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", output[i]);
//   }

//   printf("\n\nCOAES ECB DECRYPTION");
//   printf("\n128-bit key: ");
//   for (int i = 0; i < 4; i++){
//   printf("%x", key[i]);
//   } 

//   printf("\n128-bit input: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", output[i]);
//   } 

//   int de_out[16];
//   coAES_ecb_dec(key,output,de_out);
//   printf("\n128-bit output: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", de_out[i]);
//   } 
//   printf("\n");

// /////////////////////////////////////////////////////////
//   printf("\n\nCOAES CBC ENCRYPTION\n");
//   int initial_byte[16] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
//   int key0[16] = { 0x54eeff12, 0x2132eb43, 0xbbaaff23, 0x98541212};
//   printf("128-bit key: ");
//   for (int i = 0; i < 4; i++){
//   printf("%x", key0[i]);
//   } 

//   int input0[16] = { 0xcafebabe, 0xdeadbeef, 0xdeadbeef, 0x65236723};
//   printf("\n128-bit input: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", input0[i]);
//   }

//   int output0[16];
//   coAES_cbc_enc(key0,input0,output0,initial_byte,16); 
//   printf("\n128-bit output: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", output0[i]);
//   }

//   printf("\n\nCOAES CBC DECRYPTION");
//   printf("\n128-bit key: ");
//   for (int i = 0; i < 4; i++){
//   printf("%x", key0[i]);
//   } 

//   printf("\n128-bit input: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", output0[i]);
//   } 

//   int de_out0[16];
//   coAES_cbc_dec(key0,output0,de_out0,initial_byte,16);
//   printf("\n128-bit output: ");
//   for (int i = 0; i < 4; i++){
//     printf("%x", de_out0[i]);
//   } 
//   printf("\n");


  return 0;
}
