#include <stdint.h>

#include "hal.h"
#include "aes.h"
#include "simpleserial.h"

uint8_t key[16];

uint8_t get_mask(uint8_t *m, uint8_t len) {
  return 0x0;
}

uint8_t get_key(uint8_t *_k, uint8_t len) {
  int i;
  ledon(1);
  for (i=0; i<len; i++)
    key[i] = _k[i];
  return 0x0;
}

uint8_t pt_toggle = 0;
uint8_t ct[16];
uint8_t get_pt(uint8_t *_pt, uint8_t len) {
  if (pt_toggle) {
    pt_toggle = 0;
    ledoff(2);
  } else {
    pt_toggle = 1;
    ledon(2);
  }
  AES128_ECB_encrypt(_pt, key, ct);
  simpleserial_put('r', 16, ct);
  return 0x0;
}


int main(){
  uint8_t tmp[16] = {
    0x2b,0x7e,0x15,0x16,
		0x28,0xae,0xd2,0xa6,
		0xab,0xf7,0x15,0x88,
		0x09,0xcf,0x4f,0x3c
  };

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
