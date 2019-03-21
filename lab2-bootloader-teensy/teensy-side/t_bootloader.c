/* 
 * very simple bootloader.  more robust than xmodem.   (that code seems to 
 * have bugs in terms of recovery with inopportune timeouts.)
 */

#define __BOOTLOADER__
#include <stdint.h>
#include "cs140e-teensy-uart.h"
#include "t_bootloader.h"
#define BASE 0x00020000

static void die(int code) {
        // uart_send_int(code);
        // uart_println("died :(");
        error_light();
        // reboot();
}
void blink(){
    while(1)
    {
        led();
        delay();
        // for(rx=0;rx<1000000;rx++){} //dummy(rx);
        led_off();
    	delay();
        // for(rx=0;rx<1000000;rx++) {}//dummy(rx);
    }
}
//  bootloader:
//	1. wait for SOH, size, cksum from unix side.
//	2. echo SOH, checksum(size), cksum back.
// 	3. wait for ACK.
//	4. read the bytes, one at a time, copy them to ARMBASE.
//	5. verify checksum.
//	6. send ACK back.
//	7. wait 500ms 
//	8. jump to ARMBASE.
//
int bit_banging(){
	uart_init();
	// uart_println("Hello world\n");
	delay();
	error_light();
		unsigned x;
		// x = uart_get_byte();
		// x++;
		// uart_send_byte(x);
		int i;
		for (i=0; i<20; i++){
			x = uart_get_byte();
			
			x++;
			uart_send_byte(x);
		}

	return 1;
}
int notmain() {
	unsigned int code[4096];
	uart_init();
	error_light();
	// led();
	delay();

	unsigned soh = uart_get_int();
	error_light_off();
	if (soh != SOH) {
		die(NAK);
	}
	unsigned nbytes = uart_get_int();

	unsigned cksum = uart_get_int();

	// send back SOH, crc32(nbytes), checksum
	char buf[nbytes];
	unsigned cksum_size = crc32(buf, nbytes);
	// error_light();
	uart_send_int(soh);
	uart_send_int(crc32(&nbytes, sizeof(nbytes)));
	uart_send_int(cksum);

	if (uart_get_int() == ACK) {
		led();
	}
	uart_send_int(ACK);
	// error_light();

	unsigned offset;
	for (offset = 0; offset < nbytes/sizeof(unsigned); offset++){
		// unsigned *curr = (unsigned *)(BASE + offset);
		
		// int x = uart_get_int();
		code[offset] = uart_get_int();
		uart_send_int(ACK);
	}
	led_off();

	unsigned eot = uart_get_int();
	if (eot != EOT) {
		die(NAK);
		// put_uint(BAD_END);
		// reboot();
	}
	// // verify checksum
	unsigned verify_cksum = crc32(code, nbytes);
	if (verify_cksum != cksum) {

		die(NAK);
	} else {
		led();
		uart_send_int(ACK);

	}
	// led();
	// delay();
	BRANCHTO((unsigned)code);
	return 1;
}

