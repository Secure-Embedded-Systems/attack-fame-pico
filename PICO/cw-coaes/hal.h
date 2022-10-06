#ifndef HAL_H
#define HAL_H

#define reg_spictrl     (*(volatile uint32_t*)0x02000000)
#define reg_uart_clkdiv (*(volatile uint32_t*)0x02000004)
#define reg_uart_data   (*(volatile uint32_t*)0x02000008)
#define reg_gpioctrl    ( (volatile uint32_t*)0x03800000) // 8 gpio ctrls
#define reg_gpio        ( (volatile uint32_t*)0x03000000) // 8 gpios
#define reg_aes         ( (volatile uint32_t*)0x04000000) // AES coprocessor base address
#define reg_spi_master  ( (volatile uint32_t*)0x05000000) // SPI master base address
#define reg_ascon       ( (volatile uint32_t*)0x06000000) // Ascon coprocessor base address

char getch();
void putch(char c);
void trigger_high();
void trigger_low();
void platform_init();

void ledon (uint8_t v);
void ledoff(uint8_t v);
void set_flash_qspi_flag();
void set_flash_mode_spi();
void set_flash_mode_dual();
void set_flash_mode_quad();
void set_flash_mode_qddr();
void enable_flash_crm();

#endif
 
