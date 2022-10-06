#include <stdint.h>

#include "hal.h"
#include "simpleserial.h"

#define ASCON_BASE_ADDR 0x06000000 

// control register bits:
#define RESET_SOFT 0x00000001
#define DECRYPT 0x00000002
#define KEY_UPDATE 0x00000004
#define KEY_VALID 0x00000008
#define NPUB_VALID 0x00000010
#define AD_0_VALID 0x00000020
#define AD_1_VALID 0x00000040
#define DATAIN_VALID 0x00000080
#define DATAOUT_READ 0x00000100
#define TAG_READ 0x00000200
#define DATAIN_EMPTY 0x00000400
#define AD_EMPTY 0x00000800
#define AD_LAST_BLOCK 0x00001000
#define DATAIN_LAST_BLOCK 0x00002000

// status register bits:
#define KEY_READY 0x00000001            
#define NPUB_READY 0x00000002   
#define AD_0_READY 0x00000004
#define AD_1_READY 0x00000008
#define DATAIN_READY 0x00000010
#define DATAOUT_VALID 0x00000020
#define TAG_VALID 0x00000040
#define TAG_AUTH 0x00000080

int valid_bytes_ad;
int valid_bytes_dataIn;
int ad[19]; // for the longest test case
int dataIn[19]; // for the longest test case
int key[4];
int npub[4];
int ad_block_size;
int dataIn_block_size;
int dataOut[19]; // for the longest test case
int dataOut_ref [19]; // for the longest test case
int tag[4];
int tag_ref [4];

uint8_t get_key(uint8_t *_k, uint8_t len) {
  volatile int *coprocessor_base_ptr = (volatile int *)  ASCON_BASE_ADDR;
  ledon(1);
  key[0] = (_k[0] << 24)  | (_k[1] << 16)  | (_k[2] << 8)  | (_k[3]);
  key[1] = (_k[4] << 24)  | (_k[5] << 16)  | (_k[6] << 8)  | (_k[7]);
  key[2] = (_k[8] << 24)  | (_k[9] << 16)  | (_k[10] << 8) | (_k[11]);
  key[3] = (_k[12] << 24) | (_k[13] << 16) | (_k[14] << 8) | (_k[15]);

  return 0x0;
}

uint8_t pt_toggle = 0;
uint8_t get_pt(uint8_t *_pt, uint8_t len) {
  volatile int *coprocessor_base_ptr = (volatile int *)  ASCON_BASE_ADDR;
  int tmp = 0;
  int i;
  
  if (pt_toggle) {
    pt_toggle = 0;
    ledoff(2);
  } else {
    pt_toggle = 1;
    ledon(2);
  }

  // reset at the very beginning  
  coprocessor_base_ptr[0] = RESET_SOFT; // reset 
  coprocessor_base_ptr[0] = 0x00000000; // undo reset 

  // pt is used to control IV (npub)
  // test case runs ascon on a single data block of all-zero data
  dataIn_block_size = 16;
  ad_block_size     = 0;
  dataIn[0] = 0;
  dataIn[1] = 0;
  dataIn[2] = 0;
  dataIn[3] = 0;
  
  // send ad and dataIn last block size:
  coprocessor_base_ptr[11] = ((valid_bytes_dataIn & 0x00000007) << 3) |
                              (valid_bytes_ad & 0x00000007);
  
  trigger_high();
  coprocessor_base_ptr[1] = key[0]; // key_input_0
  coprocessor_base_ptr[2] = key[1]; // key_input_1
  coprocessor_base_ptr[3] = key[2]; // key_input_2
  coprocessor_base_ptr[4] = key[3]; // key_input_3
  // wait for key_ready
  while ((coprocessor_base_ptr[17] & KEY_READY) == 0); 
  // key_update, bdi_valid
  coprocessor_base_ptr[0] = KEY_UPDATE | KEY_VALID;
  trigger_low();
  
  npub[0] = (_pt[0] << 24)  | (_pt[1] << 16)  | (_pt[2] << 8)  | (_pt[3]);
  npub[1] = (_pt[4] << 24)  | (_pt[5] << 16)  | (_pt[6] << 8)  | (_pt[7]);
  npub[2] = (_pt[8] << 24)  | (_pt[9] << 16)  | (_pt[10] << 8) | (_pt[11]);
  npub[3] = (_pt[12] << 24) | (_pt[13] << 16) | (_pt[14] << 8) | (_pt[15]);

  coprocessor_base_ptr[5] = npub[0]; // npub data 0
  coprocessor_base_ptr[6] = npub[1]; // npub data 0
  coprocessor_base_ptr[7] = npub[2]; // npub data 0
  coprocessor_base_ptr[8] = npub[3]; // npub data 0
  // wait for npub_ready
  while ((coprocessor_base_ptr[17] & NPUB_READY) == 0); 
  if (ad_block_size==0 && dataIn_block_size==0) {
    return 0; // no input
  } 
  else if (ad_block_size!=0 && dataIn_block_size==0) {
    tmp = DATAIN_EMPTY;
  }
  else if (ad_block_size==0 && !dataIn_block_size==0) {
    tmp = AD_EMPTY;
  }
  
  // send npub_valid
  coprocessor_base_ptr[0] = tmp | NPUB_VALID;
  
  if (ad_block_size != 0) {
    // send ad
    for (i=0 ; i<ad_block_size ; i++) {
      if (i==ad_block_size-1) {
	tmp = tmp | AD_LAST_BLOCK;
      }
      if (i%2==0) {
	// wait for ad_0_ready
	while ((coprocessor_base_ptr[17] & AD_0_READY) == 0); 
	// send AD block
	coprocessor_base_ptr[9] = ad[2*i];
	coprocessor_base_ptr[10] = ad[2*i+1];
	coprocessor_base_ptr[0] = tmp | AD_0_VALID;
      } else {
	// wait for ad_1_ready
	while ((coprocessor_base_ptr[17] & AD_1_READY) == 0); 
	// send AD block
	coprocessor_base_ptr[9] = ad[2*i];
	coprocessor_base_ptr[10] = ad[2*i+1];
	coprocessor_base_ptr[0] = tmp | AD_1_VALID;
      }
    }
  }

  if (dataIn_block_size != 0) {
    // send dataIn and read dataOut
    for (i=0 ; i<dataIn_block_size ; i++) {
      if (i==dataIn_block_size-1) {
	tmp = tmp | DATAIN_LAST_BLOCK;
      }
      // wait for datain_ready
      while ((coprocessor_base_ptr[17] & DATAIN_READY) == 0); 
      // send datain block
      coprocessor_base_ptr[9] = dataIn[2*i];
      coprocessor_base_ptr[10] = dataIn[2*i+1];
      // send datain_valid
      coprocessor_base_ptr[0] = tmp | DATAIN_VALID;
      
      // wait for dataout_valid
      while ((coprocessor_base_ptr[17] & DATAOUT_VALID) == 0); 
      
      // read dataOut
      dataOut[2*i] = coprocessor_base_ptr[11];
      dataOut[2*i+1] = coprocessor_base_ptr[12];
      // send dataout_read
      coprocessor_base_ptr[0] = tmp | DATAOUT_READ;
    }
  }

  // wait for tag_valid
  while ((coprocessor_base_ptr[17] & TAG_VALID) == 0); 
  
  tag[0] = coprocessor_base_ptr[13];
  tag[1] = coprocessor_base_ptr[14];
  tag[2] = coprocessor_base_ptr[15];
  tag[3] = coprocessor_base_ptr[16];
  
  // send tag_read
  coprocessor_base_ptr[0] = TAG_READ;

  uint8_t ct[16];
  ct[0]  = (tag[0] >> 24) & 0xff;
  ct[1]  = (tag[0] >> 16) & 0xff;
  ct[2]  = (tag[0] >>  8) & 0xff;
  ct[3]  = (tag[0]      ) & 0xff;
  ct[4]  = (tag[1] >> 24) & 0xff;
  ct[5]  = (tag[1] >> 16) & 0xff;
  ct[6]  = (tag[1] >>  8) & 0xff;
  ct[7]  = (tag[1]      ) & 0xff;
  ct[8]  = (tag[2] >> 24) & 0xff;
  ct[9]  = (tag[2] >> 16) & 0xff;
  ct[10] = (tag[2] >>  8) & 0xff;
  ct[11] = (tag[2]      ) & 0xff;
  ct[12] = (tag[3] >> 24) & 0xff;
  ct[13] = (tag[3] >> 16) & 0xff;
  ct[14] = (tag[3] >>  8) & 0xff;
  ct[15] = (tag[3]      ) & 0xff;
  
  simpleserial_put('r', 16, ct);
  return 0x0;
}

int main(){
  platform_init();

  int i;
  volatile int j;
  for (i=0; i<8; i++) {
    ledon(i);
    for (j=0; j<(1<<14); ) j++;
    ledoff(i);
  }
  
  simpleserial_init();
  simpleserial_addcmd('k', 16, get_key);
  simpleserial_addcmd('p', 16, get_pt);

  while (1) {
    ledon(0);
    simpleserial_get();
  }
  
}
