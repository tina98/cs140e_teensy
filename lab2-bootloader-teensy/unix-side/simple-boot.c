#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "demand.h"
#include "trace.h"

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

static void send_byte(int fd, unsigned char b) {
	if(write(fd, &b, 1) < 0)
		panic("write failed in send_byte\n");
}
static unsigned char get_byte(int fd) {
	unsigned char b;
	int n;
	if((n = read(fd, &b, 1)) != 1)
		// printf("Can't read, try again?\n");
		panic("read failed in get_byte: expected 1 byte, got %d\n",n);
	return b;
}

// NOTE: the other way to do is to assign these to a char array b and 
//	return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
// 	(get_byte(fd) | get_byte(fd) << 8 ...) 
// isn't guaranteed to be called in that order b/c | is not a seq point.
static unsigned get_uint(int fd) {
        unsigned u = get_byte(fd);
        u |= get_byte(fd) << 8;
        u |= get_byte(fd) << 16;
        u |= get_byte(fd) << 24;
        // trace_read32(u);
        return u;
}

void put_uint(int fd, unsigned u) {
	// mask not necessary.
		// trace_write32(u);
        send_byte(fd, (u >> 0)  & 0xff);
        send_byte(fd, (u >> 8)  & 0xff);
        send_byte(fd, (u >> 16) & 0xff);
        send_byte(fd, (u >> 24) & 0xff);
}

// simple utility function to check that a u32 read from the 
// file descriptor matches <v>.
void expect(const char *msg, int fd, unsigned v) {
	unsigned x = get_uint(fd);
	if(x != v) {
		put_uint(fd, NAK);
		panic("%s: expected %x, got %x\n", msg, v,x);
	}
}
void bit_banging(int fd, const unsigned char* buf, unsigned n){
	int i, x=1; //x=1;
	printf("send!!\n");
	send_byte(fd, x);
	for (i=0;i<20; i++){
		x = get_byte(fd);
		printf("Received byte %d\n", x);
		x++;
		send_byte(fd, x);
		printf("Send byte %d\n", x);
	}	
	return;
}
// unix-side bootloader: send the bytes, using the protocol.
// read/write using put_uint() get_unint().
void simple_boot(int fd, const unsigned char * buf, unsigned n) { 

	// send SOH
	// printf("Sending SOH\n");

	// unsigned char x = 1;
	// int res=0;
	// while (res == 0) {
	// 	printf("try\n");
	// 	send_byte(fd, x);
	// 	res=get_byte(fd);
	// 	printf("Got res %d\n", res);
	// }

	// printf("Send byte successfully%d\n", x);

	// int x=1;
	// return;
	// printf("send byte\n");
	put_uint(fd, SOH);
	// send file size
	put_uint(fd, n);
	// send checksum
	u32 cksum= crc32(buf, n);
	put_uint(fd, cksum);
	printf("Send SOH, file size, cksum\n");

	//verify echoed SOH, crc32 checksum of nbytes, checksum back
	expect("Should be SOH", fd, SOH);
	printf("got soh!!\n");
	expect("Should be CRC32(nbytes)", fd, crc32(&n, sizeof(unsigned)));
	printf("got crc32 right!!\n");
	expect("Should be checksum", fd, cksum);
	printf("got checksum!!! putting ACK\n");
	put_uint(fd, ACK);
	expect("Should be ACK", fd, ACK);

	// u32 ack;
	printf("sending %d bytes of code now!\n", n);
	// usleep(100);
	//copy over binary code

	for (unsigned offset = 0; offset < n; offset += sizeof(unsigned)){
		unsigned *curr = (unsigned *)(buf + offset);
		// printf("%x\n", *curr);
		put_uint(fd, *curr);
		expect("Should be ACK", fd, ACK);
		// ack = get_uint(fd);
		// if (ack != ACK) {
		// 	printf("fuck\n");
		// } else {
		// 	printf(".");
		// }
	}
	// send EOT
	put_uint(fd, EOT);

	// wait for ACK
	printf("should be getting ackkkk\n");
	expect("Should be ACK", fd, ACK);

	printf("Everything done!!!!\n");

}
