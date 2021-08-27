CC=/opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf-gcc
GRMON=/opt/grmon-eval-3.2.11/linux/bin64/grmon
OBJCOPY=/opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf-objcopy
OBJDUMP=/opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf-objdump
MKPROM2=/opt/mkprom2/mkprom2
SIZE=/opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf-size

swaes: aes.c
	$(CC) aes.c -o swaes

prom.out: aes
		$(MKPROM2) -v \
				-msoft-float \
				-leon3 \
				-freq 8 \
				-nocomp \
				-rstaddr 0x10000000 \
				-baud 38461 \
				-gpt 0x80000600  \
				-o prom.out \
				-ramsize 128 \
				-ccprefix /opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf \
				-dump aes

prom.bin: prom.out
		$(SIZE) prom.out
		$(OBJCOPY) -O binary prom.out ./prom.bin
		$(OBJDUMP) -D prom.out >prom.s
		xxd prom.bin > prom.txt

full.bin: prom.bin
		cp prom.bin full.bin
		truncate -s 4194304 full.bin

flashwrite_buspirate: full.bin
		flashrom  -p buspirate_spi:dev=/dev/ttyUSB0,spispeed=2M --layout fame.layout --image block1 -c AT45DB321D --write full.bin              

flashverify_buspirate: 
		flashrom -p buspirate_spi:dev=/dev/ttyUSB0,spispeed=2M -c AT45DB321D --read verify.bin

flasherase_buspirate:
		flashrom -p buspirate_spi:dev=/dev/ttyUSB0,spispeed=2M -c AT45DB321D --erase

flashwrite_ftdi: full.bin
		flashrom  -p ft2232_spi:type=4232H,port=A --layout fame.layout --image block1 -c AT45DB321D --write full.bin            

flashverify_ftdi: 
		flashrom -p ft2232_spi:type=4232H,port=A -c AT45DB321D --read verify.bin

flasherase_ftdi:
		flashrom -p  ft2232_spi:type=4232H,port=A -c AT45DB321D --erase

clean:
	rm -f *.o aes

# debugging
grmon:
	sudo $(GRMON) -uart /dev/ttyUSB2 -u -baud 38400

# this must be done once after board turn-on
#CLOCKSETTINGS=../clockconf/SI5351A_4MHZ.txt
CLOCKSETTINGS=../clockconf/SI5351A_8MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_20MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_30MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_40MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_50MHZ.txt
init:
	sudo ../clockconf/unbindports.sh
	sudo ../clockconf/clockconf $(CLOCKSETTINGS)

clockconf:
	sudo ../clockconf/clockconf $(CLOCKSETTINGS)
