#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#include "my-teensy-install.h"
#include "../../lib_teensy/cs140e-teensy-tty.h"

#define MAX_MEMORY_SIZE 0x100000
static unsigned char firmware_image[MAX_MEMORY_SIZE];
static unsigned char firmware_mask[MAX_MEMORY_SIZE];

int main(int argc, char **argv) {
	printf("Teensy loader-by Tina Li, CS140E\n\n");

	if (argc != 2) return die("should be running \"my-teensy-install [filename].hex\"");

	const char* filename = argv[1];
	printf("Loading %s to Teensy...\n", filename);

	//for mk20dx256 MCU, Teensy 3.2
	int code_size = 262144; 
	int block_size = 1024;

	int write_size = block_size + 64; //first 64 bits have address + buffer of 0s

	// reading hex file
	int num = read_hex(filename);
	if (num < 0)  return die("Error reading hex file");
	printf("Read %s: %d bytes, %.1f%% used\n", filename, num, (double)num/(double)code_size*100.0);

	int fd = 0;

	// opening the Teensy
	while (1) {
		fd = teensy_open();
		if (fd) break;
		printf("Can't open Teensy... waiting to press reset...\n");
		delay(0.25);
	}
	teensy_write(fd, "hello!!!!\n", 10, 5.0);

	// // transfer data from hex file to Teensy
	// unsigned addr;
	// unsigned char buf[2048];

	// printf("%s\n\n\n\n",buf);

	// int first_block = 1;


	// for (addr = 0; addr < code_size; addr += block_size) {
	// 	memset(buf, 0, 2048);
	// 	if (!first_block && !hex_bytes_within_range(addr, addr+block_size-1))
	// 		continue;

	// 	// if (!first_block && memory_is_blank(addr, block_size)) continue;

	// 	buf[0] = addr & 255;
	// 	buf[1] = (addr>>8) & 255;
	// 	buf[2] = (addr>>16) & 255;

	// 	printf("%d blocksize %d\n", write_size, sizeof(int));
	// 	memset(buf+3, 0, 61);
	// 	hex_get_data(addr, block_size, buf+64);
		

	// 	if (teensy_write(fd, buf, write_size, first_block ? 5.0 : 0.5) < 0)
	// 		return die("can't write to Teensy");

	// 	first_block = 0;
	// }

	// teensy_boot(fd, buf, write_size);
	// return teensy_close(fd);

}

void delay(double seconds) {
	usleep(seconds * 1000000.0);
}

int die(const char* error_msg) {
	printf("ERROR: %s... will reboot.\n", error_msg);
	teensy_reboot();
	return -1;
}

static void send_byte(int fd, unsigned char b) {
	if(write(fd, &b, 1) < 0)
		die("write failed in send_byte\n");
}

void put_uint(int fd, unsigned u) {
    send_byte(fd, (u >> 0)  & 0xff);
    send_byte(fd, (u >> 8)  & 0xff);
    send_byte(fd, (u >> 16) & 0xff);
    send_byte(fd, (u >> 24) & 0xff);
}

// read the hex file and make sure it's valid
int read_hex(const char* filename) {
	//file pointer from fopen
	FILE *fp;

	int i = 0;
	char buf[1024];
	int byte_count = 0;

	for (i = 0; i < MAX_MEMORY_SIZE; i++) {
		firmware_image[i] = 0xFF;
		firmware_mask[i] = 0;
	}

	fp = fopen(filename, "r"); //not using open because we want buffered IO to parse hex
	if (fp == NULL) return -1;

	// while not EOF, read each line into buf, parse line
	while (!feof(fp)) {
		*buf = '\0';
		if (!fgets(buf, sizeof(buf), fp)) break;
		if (*buf){
			int result = parse_hex(buf, &byte_count);

			if (result == -1) return -1; // error with parsing
			else if (result == -2) break; // found end of file
		}
		if (feof(stdin)) break;
	}

	// fclose(fp);

	return byte_count;
}

// parse each line to make sure it's a valid hex line
int parse_hex(char* buf, int *byte_count) {
	// normal hex line: :100670007A7101FB03F3D31A3B600E20012100F0D5
	//                   ++----**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!!
	// ++ = number bytes, ---- = start address, ** = code, ~ = data, !! = checksum
	// code = 00 if data, 01 = EOF, 02/04=files higher than 64kb
	// 11 is special because that's the number of bytes that's NOT the data
	int len=0, addr=0, code=0, our_cksum=0, cksum=0;


	// check if first char is ":", check strlen > mandatory parts, check length,
	// check if total line length > mandatory parts, check start addr, check code
	if (buf[0] != ':' || strlen(buf) < 11 || !sscanf(buf+1, "%02x", &len) ||
		strlen(buf) < 11 + 2 * len || !sscanf(buf+3, "%04x", &addr) ||
		!sscanf(buf+7, "%02x", &code) || addr + len >= MAX_MEMORY_SIZE) 
		return -1;


	// add mandatory parts for checksum first
	our_cksum = (len&255) + ((addr>>8)&255) + (addr&255) + (code&255);
	if (code != 0) {
		if (code == 1) return -2;
		else return 0;
	}

	*byte_count = *byte_count + len;

	// move pointer to the data part
	char* ptr=buf+9;
	int data = 0;

	int num = 0;

	//update the firmware_image andfirmware_mask
	while (num != len) {
		if (!sscanf(ptr, "%02x", &data)) return -1;
		data = data&255;

		// actual bytes of data
		firmware_image[addr + num] = data;
		// whether there is data
		firmware_mask[addr + num] = 1;

		ptr += 2;
		our_cksum += data;

		num++;
		if (num>=256) return -1;
	}

	// if the checksum is correct
	if (!sscanf(ptr, "%02x", &cksum)) return -1;
	if ((our_cksum & 255) + (cksum&255) & 255) return -1;

	return 0;
}

void hex_get_data(int addr, int len, unsigned char *bytes) {
	int i;
	if (addr < 0 || len < 0 || addr + len>=MAX_MEMORY_SIZE) {
		for (i=0; i<len; i++){
			bytes[i] = 255;
		}
		return;
	}
	for (i=0; i<len; i++){
		if (firmware_mask[addr]) {
			bytes[i] = firmware_image[addr];
		} else {
			bytes[i] = 255;
		}
		addr++;
	}
}

int hex_bytes_within_range(int start, int end) {
	if (start<0 || start>=MAX_MEMORY_SIZE || 
		end<0 || end>=MAX_MEMORY_SIZE || start > end)
			return 0;

	int i;
	// make sure there's a least some data in there, not all 0s
	for (i = start; i<=end; i++){
		if (firmware_mask[i]) return 1;
	}
	return 0;
}


// returns a file descriptor for the Teensy (USB)
int teensy_open() {
	char *portname = 0;
	int fd;

	fd = open_tty(&portname);

	// set it to be 8n1  and 115200 baud
	fd = set_tty_to_8n1(fd, B115200, 1.0);
	if (fd<0) die("opening Teensy failed");

	printf("Successfully connected to Teensy\n");
	return fd;
}

int teensy_write(int fd, unsigned char *buf, int len, double timeout) {
	printf("Teensy write len=%d %d\n", len, sizeof(unsigned));
	unsigned offset = 0;
	time_t current_time = time(NULL);

	for (offset = 0; offset < len; offset++){
		put_uint(fd, buf[offset]);
		if (time(NULL)-current_time > timeout) 
			return die("timed out");
	}
	printf("Finished writing\n");
	return 0;


}

int teensy_close(int fd) {
	if (close(fd) < 0) return die("close syscall failed");
	return 0;
}

int teensy_boot(int fd, unsigned char *buf, int write_size) {
	printf("BOOTING!!!\n");
	memset(buf, 0, write_size);
	buf[0] = 0xFF;
	buf[1] = 0xFF;
	buf[2] = 0xFF;
	teensy_write(fd, buf, write_size, 0.5);
	return 1;
}
void teensy_reboot() {
	printf("REBOOTING!!!!!\n");
}