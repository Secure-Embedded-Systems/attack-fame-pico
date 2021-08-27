CC=/opt/bcc-2.2.0-gcc/bin/sparc-gaisler-elf-gcc
GRMON=/opt/grmon-eval-3.2.11/linux/bin64/grmon

coaes: coAES.c
	$(CC) coAES.c -o coaes

clean:
	rm -f *.o coaes

# debugging
grmon:
	sudo $(GRMON) -uart /dev/ttyUSB3 -u -baud 19200

# this must be done once after board turn-on
CLOCKSETTINGS=../clockconf/SI5351A_1MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_4MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_8MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_20MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_30MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_40MHZ.txt
#CLOCKSETTINGS=../clockconf/SI5351A_50MHZ.txt
init:
	sudo ../clockconf/unbindports.sh
	sudo ../clockconf/clockconf $(CLOCKSETTINGS)

clockconf:
	sudo ../clockconf/clockconf $(CLOCKSETTINGS)
