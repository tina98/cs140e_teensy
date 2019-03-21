
# ARMGNU = arm-none-eabi
# #ARMGNU = arm-none-linux-gnueabi
# CC = $(ARMGNU)-gcc

# AOPS = --warn --fatal-warnings 
# # COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
# COPS = -Wall -O -nostdlib -nostartfiles -ffreestanding 
# all : t_bootloader.hex 

# vectors.o : vectors.s
# 	$(ARMGNU)-as vectors.s -o vectors.o

# t_bootloader.o : t_bootloader.c
# 	$(ARMGNU)-gcc $(COPS) -mthumb -c t_bootloader.c -o t_bootloader.o

# t_bootloader.hex : memmap vectors.o cs140e-teensy-uart.o t_bootloader.o 
# 	$(ARMGNU)-ld -o t_bootloader.elf -T memmap vectors.o cs140e-teensy-uart.o t_bootloader.o 
# 	$(ARMGNU)-objdump -D t_bootloader.elf > t_bootloader.list
# 	$(ARMGNU)-objcopy t_bootloader.elf t_bootloader.hex -O ihex

# clean:
# 	rm -f *.bin
# 	rm -f *.hex
# 	rm -f *.o
# 	rm -f *.elf
# 	rm -f *.list

# ARMGNU = arm-none-eabi
# #ARMGNU = arm-none-linux-gnueabi

# AOPS = --warn --fatal-warnings 
# COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 


# all : t_bootloader.gcc.thumb.hex

# vectors.o : vectors.s
# 	$(ARMGNU)-as vectors.s -o vectors.o

# t_bootloader.gcc.thumb.o : t_bootloader.c
# 	$(ARMGNU)-gcc $(COPS) -mthumb -c t_bootloader.c -o t_bootloader.gcc.thumb.o

# t_bootloader.gcc.thumb.hex : memmap vectors.o cs140e-teensy-uart.o t_bootloader.gcc.thumb.o
# 	$(ARMGNU)-ld -o t_bootloader.gcc.thumb.elf -T memmap vectors.o cs140e-teensy-uart.o t_bootloader.gcc.thumb.o
# 	$(ARMGNU)-objdump -D t_bootloader.gcc.thumb.elf > t_bootloader.gcc.thumb.list
# 	$(ARMGNU)-objcopy t_bootloader.gcc.thumb.elf t_bootloader.gcc.thumb.hex -O ihex

# clean:
# 	rm -f *.bin
# 	rm -f *.hex
# 	rm -f *.o
# 	rm -f *.elf
# 	rm -f *.list

ARMGNU = arm-none-eabi
#ARMGNU = arm-none-linux-gnueabi

AOPS = --warn --fatal-warnings 
# -Werror 
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


all : t_bootloader.gcc.thumb.hex

vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

t_bootloader.gcc.thumb.o : t_bootloader.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c t_bootloader.c -o t_bootloader.gcc.thumb.o

cs140e-teensy-uart.gcc.thumb.o : cs140e-teensy-uart.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c cs140e-teensy-uart.c -o cs140e-teensy-uart.gcc.thumb.o

t_bootloader.gcc.thumb.hex : memmap vectors.o cs140e-teensy-uart.gcc.thumb.o t_bootloader.gcc.thumb.o
	$(ARMGNU)-ld -o t_bootloader.gcc.thumb.elf -T memmap vectors.o cs140e-teensy-uart.gcc.thumb.o t_bootloader.gcc.thumb.o
	$(ARMGNU)-objdump -D t_bootloader.gcc.thumb.elf > t_bootloader.gcc.thumb.list
	$(ARMGNU)-objcopy t_bootloader.gcc.thumb.elf t_bootloader.gcc.thumb.hex -O ihex

clean:
	rm -f *.bin
	rm -f *.hex
	rm -f *.o
	rm -f *.elf
	rm -f *.list