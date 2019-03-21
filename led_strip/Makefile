
ARMGNU = arm-none-eabi
#ARMGNU = arm-none-linux-gnueabi

AOPS = --warn --fatal-warnings 
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 

all : blinker01.hex blinker01.bin

vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

blinker01.o : blinker01.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c blinker01.c -o blinker01.o

blinker01.hex : memmap vectors.o blinker01.o
	$(ARMGNU)-ld -o blinker01.elf -T memmap vectors.o blinker01.o
	$(ARMGNU)-objdump -D blinker01.elf > blinker01.list
	$(ARMGNU)-objcopy blinker01.elf blinker01.hex -O ihex

blinker01.bin : memmap vectors.o blinker01.o
	$(ARMGNU)-ld -o blinker01.elf -T memmap vectors.o blinker01.o
	$(ARMGNU)-objdump -D blinker01.elf > blinker01.list
	$(ARMGNU)-objcopy -O binary blinker01.elf blinker01.bin

clean:
	rm -f *.bin
	rm -f *.hex
	rm -f *.o
	rm -f *.elf
	rm -f *.list

