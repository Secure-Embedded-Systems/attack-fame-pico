CROSSCPP= /opt/riscv/bin/riscv32-unknown-elf-cpp 
CROSSGCC= /opt/riscv/bin/riscv32-unknown-elf-gcc 
CROSSOBJDUMP= /opt/riscv/bin/riscv32-unknown-elf-objdump 
CROSSOBJCOPY= /opt/riscv/bin/riscv32-unknown-elf-objcopy
CROSSSIZE= /opt/riscv/bin/riscv32-unknown-elf-size
CFLAGS=
CLOCK_FREQ?=1

all: picoboot.s picoapp.s firmware.bin

picoboot.elf: picoboot.c memorymap.ld bootloader.ld
	$(CROSSGCC) $(CFLAGS) \
			-D PICOCHIP \
			-Dmarch=rv32ic \
			-Wl,-Bstatic,-T,bootloader.ld,--strip-debug \
			-ffreestanding \
			-nostdlib \
			-o picoboot.elf \
			start.s \
			picoboot.c
	$(CROSSSIZE) picoboot.elf

picoboot.s: picoboot.elf
	$(CROSSOBJDUMP) --disassemble picoboot.elf > picoboot.s

picoboot.bin: picoboot.elf
	$(CROSSOBJCOPY) -O binary --pad-to 0x110000 --gap-fill 0xFF picoboot.elf picoboot.bin

picoapp.elf: start.s aes.c hal.c simpleserial.c top.c memorymap.ld app.ld
	$(CROSSGCC) $(CFLAGS) \
			-D PICOCHIP \
			-Dmarch=rv32ic \
			-Wl,-Bstatic,-T,app.ld,--strip-debug \
			-ffreestanding \
			-nostdlib \
			-o picoapp.elf \
			start.s \
			aes.c \
			hal.c \
			simpleserial.c \
			top.c
	$(CROSSSIZE) picoapp.elf

picoapp.s: picoapp.elf
	$(CROSSOBJDUMP) --disassemble picoapp.elf > picoapp.s

picoapp.bin: picoapp.elf
	$(CROSSOBJCOPY) -O binary picoapp.elf picoapp.bin

firmware.bin: picoboot.bin picoapp.bin
	cat $^ > $@

clean:
	rm -f picoapp.elf picoapp.s picoboot.elf picochip.elf picoboot.bin picoapp.bin picoboot.s firmware.bin picochip.layout sections.lds picochip_fw* *~

#----------------------------------------
# flash chip programming

writeflash: firmware.bin clockconf
	@rm -f picochip.layout
	@touch picochip.layout
	@echo "00000000:000fffff start" >> picochip.layout
	@echo "00100000:001fffff program" >> picochip.layout
	@echo "00200000:00ffffff remainder" >> picochip.layout
	@truncate -s 1M zeros.bin
	@cat zeros.bin firmware.bin > concat.bin
	@truncate -s 16M concat.bin
	@rm zeros.bin
	flashrom -p ft2232_spi:type=4232H,port=A --layout picochip.layout --image program --write concat.bin
	@rm concat.bin

ifeq ($(CLOCK_FREQ), 1)
   CLOCKSETTINGS=../clockconf/cdce925_1mhz.txt
endif

ifeq ($(CLOCK_FREQ), 4)
   CLOCKSETTINGS=../clockconf/cdce925_4mhz.txt
endif

ifeq ($(CLOCK_FREQ), 8)
   CLOCKSETTINGS=../clockconf/cdce925_8mhz.txt
endif

ifeq ($(CLOCK_FREQ), 10)
   CLOCKSETTINGS=../clockconf/cdce925_10mhz.txt
endif

ifeq ($(CLOCK_FREQ), 16)
   CLOCKSETTINGS=../clockconf/cdce925_16mhz.txt
endif

ifeq ($(CLOCK_FREQ), 40)
   CLOCKSETTINGS=../clockconf/cdce925_40mhz.txt
endif

clockconf:
	sudo ../clockconf/clockconf $(CLOCKSETTINGS)
