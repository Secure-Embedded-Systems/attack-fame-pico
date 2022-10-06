#include <stdint.h>
#include <string.h>

extern int __bootrom_start__;
extern int __bootrom_size__;
extern int __approm_start__;
extern int __approm_size__;
extern int __appram_start__;
extern int __appram_size__;
extern int __RAMSIZE__;
extern int __RAMAPPSIZE__;

#define reg_gpioctrl ((volatile uint32_t*)0x03800000) // 8 gpio ctrls
#define reg_gpio     ((volatile uint32_t*)0x03000000) // 8 gpios

void ledoff() {
  reg_gpioctrl[0] = 1;
  reg_gpioctrl[1] = 1;
  reg_gpioctrl[2] = 1;
  reg_gpioctrl[3] = 1;
  reg_gpioctrl[4] = 1;
  reg_gpioctrl[5] = 1;
  reg_gpioctrl[6] = 1;
  reg_gpioctrl[7] = 1;
  reg_gpio[0] = 0;
  reg_gpio[1] = 0;
  reg_gpio[2] = 0;
  reg_gpio[3] = 0;
  reg_gpio[4] = 0;
  reg_gpio[5] = 0;
  reg_gpio[6] = 0;
  reg_gpio[7] = 0;
}

void ledon(int v) {
  reg_gpio[v] = 1;
}

int main() {
  uint32_t *src = (uint32_t *) &__approm_start__;  // copy from flash app region
  uint32_t *dst = (uint32_t *) &__appram_start__;  //      to RAM app region
  int size = (int) &__appram_size__;               // for max RAM # bytes
  int sizew = (size >> 2);
  int sizewstep = (sizew >> 3);
  int theled = 0;
  int nextstep = sizewstep;
  ledoff();                                        // turn on all leds
  for (int i=0; i< sizew; i++) {                   // word-level copy
    *dst++ = *src++;
    if (i == nextstep) {
      ledon(theled++);
      nextstep += sizewstep;
    }
  }
  
  /* reset stack ptr to top of ram */
  __asm("mv x2, %0" : /* no output */ : "r" (&__RAMSIZE__) : /* ignore clobber */);

  /* jump to application */
  __asm("mv x1, %0" : /* no output */ : "r" (&__appram_start__) : /* ignore clobber */);
  __asm("ret" : /* no output */ : /* no input */ : /* no clobber */);
}


  /*

  // for 8-bit copy use

  uint8_t *src = (uint8_t *) &__approm_start__;
  uint8_t *dst = (uint8_t *) &__appram_start__;
  int size = (int) &__appram_size__;
  ledon();
  for (int i=0; i<size; i++) {
    *dst++ = *src++;
    if ((i & 0x000) == 0)
      ledoff(i >> 12);
  }
  */

