#include <stdint.h>

#include "hal.h"
#include "simpleserial.h"

uint32_t key[4];

uint8_t get_mask(uint8_t *m, uint8_t len) {
  return 0x0;
}

uint8_t get_key(uint8_t *_k, uint8_t len) {
  volatile int *coprocessor_base_ptr = (volatile int *)  0x04000000;
  ledon(1);
  key[0] = (_k[0] << 24)  | (_k[1] << 16)  | (_k[2] << 8)  | (_k[3]);
  key[1] = (_k[4] << 24)  | (_k[5] << 16)  | (_k[6] << 8)  | (_k[7]);
  key[2] = (_k[8] << 24)  | (_k[9] << 16)  | (_k[10] << 8) | (_k[11]);
  key[3] = (_k[12] << 24) | (_k[13] << 16) | (_k[14] << 8) | (_k[15]);

  coprocessor_base_ptr[1] = key[0];
  coprocessor_base_ptr[2] = key[1];
  coprocessor_base_ptr[3] = key[2];
  coprocessor_base_ptr[4] = key[3];

  return 0x0;
}

uint8_t pt_toggle = 0;
uint8_t ct[16];
uint8_t get_pt(uint8_t *_pt, uint8_t len) {
  uint32_t pt[4];
  volatile int *coprocessor_base_ptr = (volatile int *)  0x04000000;

  if (pt_toggle) {
    pt_toggle = 0;
    ledoff(2);
  } else {
    pt_toggle = 1;
    ledon(2);
  }

  pt[0] = (_pt[0] << 24)  | (_pt[1] << 16)  | (_pt[2] << 8)  | (_pt[3]);
  pt[1] = (_pt[4] << 24)  | (_pt[5] << 16)  | (_pt[6] << 8)  | (_pt[7]);
  pt[2] = (_pt[8] << 24)  | (_pt[9] << 16)  | (_pt[10] << 8) | (_pt[11]);
  pt[3] = (_pt[12] << 24) | (_pt[13] << 16) | (_pt[14] << 8) | (_pt[15]);

  coprocessor_base_ptr[5] = pt[0];	
  coprocessor_base_ptr[6] = pt[1];	
  coprocessor_base_ptr[7] = pt[2];	
  coprocessor_base_ptr[8] = pt[3];	

  trigger_high();
  coprocessor_base_ptr[0] = 0x00000006;	   // control_register
  coprocessor_base_ptr[0] = 0x00000004;	   // control_register valid clear
  while (coprocessor_base_ptr[17] != 0x1); // poll for completion
  trigger_low();

  uint32_t _ct[4];
  _ct[0] = coprocessor_base_ptr[13];
  _ct[1] = coprocessor_base_ptr[14];
  _ct[2] = coprocessor_base_ptr[15];
  _ct[3] = coprocessor_base_ptr[16];

  ct[0]  = (_ct[0] >> 24) & 0xff;
  ct[1]  = (_ct[0] >> 16) & 0xff;
  ct[2]  = (_ct[0] >>  8) & 0xff;
  ct[3]  = (_ct[0]      ) & 0xff;
  ct[4]  = (_ct[1] >> 24) & 0xff;
  ct[5]  = (_ct[1] >> 16) & 0xff;
  ct[6]  = (_ct[1] >>  8) & 0xff;
  ct[7]  = (_ct[1]      ) & 0xff;
  ct[8]  = (_ct[2] >> 24) & 0xff;
  ct[9]  = (_ct[2] >> 16) & 0xff;
  ct[10] = (_ct[2] >>  8) & 0xff;
  ct[11] = (_ct[2]      ) & 0xff;
  ct[12] = (_ct[3] >> 24) & 0xff;
  ct[13] = (_ct[3] >> 16) & 0xff;
  ct[14] = (_ct[3] >>  8) & 0xff;
  ct[15] = (_ct[3]      ) & 0xff;
  
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
