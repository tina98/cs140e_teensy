/* 
 * very simple bootloader.  more robust than xmodem.   (that code seems to 
 * have bugs in terms of recovery with inopportune timeouts.)
 */

#define __BOOTLOADER__
#include <stdint.h>
#include "cs140e-teensy-uart.h"
#include "t_bootloader.h"

static void die(int code) {
        error_light();
}
void blink(){
    while(1)
    {
        led();
        delay();
        led_off();
    	delay();
    }
}

// increment a byte that the Unix had sent
int bit_banging(){
	uart_init();

	// delay to make sure everything is set up
	delay();
	// turn on light to make sure it's working
	error_light();

	unsigned x;
	int i;
	// get byte, increment, send it back
	for (i=0; i<20; i++){
		x = uart_get_byte();
		
		x++;
		uart_send_byte(x);
	}

	return 1;
}

//  bootloader:
	// 1. wait for SOH, size, cksum from unix side.
	// 2. echo SOH, checksum(size), cksum back.
	// 3. wait for ACK.
	// 4. read the bytes, one at a time, copy them to code[].
	// 5. verify checksum.
	// 6. send ACK back.
	// 7. jump to code[].
//
int notmain() {
	unsigned int code[4096];
	uart_init();
	delay();

	// wait for SOH, size, checksum
	unsigned soh = uart_get_int();
	error_light_off();
	if (soh != SOH) {
		die(NAK);
	}
	unsigned nbytes = uart_get_int();
	unsigned cksum = uart_get_int();

	// send back SOH, crc32(nbytes), checksum
	uart_send_int(soh);
	uart_send_int(crc32(&nbytes, sizeof(nbytes)));
	uart_send_int(cksum);

	// wait for ACK, turn LED on we know it' transmitting
	if (uart_get_int() == ACK) {
		led();
	}
	// send back ACK
	uart_send_int(ACK);

	// save code in buffer, send back ACK every time
	unsigned offset;
	for (offset = 0; offset < nbytes/sizeof(unsigned); offset++){
		code[offset] = uart_get_int();
		uart_send_int(ACK);
	}
	// code down transmitting, so turn off LED
	led_off();

	// Get EOT
	unsigned eot = uart_get_int();
	if (eot != EOT) {
		die(NAK);
	}

	// Verify checksum
	unsigned verify_cksum = crc32(code, nbytes);
	if (verify_cksum != cksum) {
		die(NAK);
	} else {
		// send back ACK
		uart_send_int(ACK);
	}
	
	// branch to the new code!
	BRANCHTO((unsigned)code);
	return 1;
}

