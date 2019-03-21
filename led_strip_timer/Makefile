ARMGNU = arm-none-eabi
#ARMGNU = arm-none-linux-gnueabi

AOPS = --warn --fatal-warnings 
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 

all : timer_interrupt.hex 

vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

timer_interrupt.o : timer_interrupt.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c timer_interrupt.c -o timer_interrupt.o

timer_interrupt.hex : memmap vectors.o timer_interrupt.o
	$(ARMGNU)-ld -o timer_interrupt.elf -T memmap vectors.o timer_interrupt.o
	$(ARMGNU)-objdump -D timer_interrupt.elf > timer_interrupt.list
	$(ARMGNU)-objcopy timer_interrupt.elf timer_interrupt.hex -O ihex

clean:
	rm -f *.bin
	rm -f *.hex
	rm -f *.o
	rm -f *.elf
	rm -f *.list

