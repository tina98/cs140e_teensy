#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "demand.h"
#include "../shared-code/simple-boot.h"
#include "support.h"

#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

// read entire file into buffer.  return it, write totat bytes to <size>
unsigned char *read_file(int *size, const char *name) {
	// get file descriptor
	int fd = open(name, O_RDONLY);
	if (fd < 0) panic("Invalid file name.\n");

	// get the file size from the statistics
	struct stat file_stat;
	if (stat(name, &file_stat) < 0) panic("Invalid file name.\n");
	
	ssize_t file_size = file_stat.st_size;
	size_t rounded_file_size = roundup(file_size, 4);

	// malloc the buffer to send back
	unsigned char* buf = malloc(rounded_file_size);

	int err = read(fd, buf, file_size);
	if (err != file_size) panic("Error reading in file\n");

	*size = rounded_file_size;
	return buf;
}

#define _SVID_SOURCE
#include <dirent.h>
const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
	"cu.SLAB_USB", // mac os
	0
};

int prefix_filter(const struct dirent *entry) {
	for (int i = 0; i < (sizeof(ttyusb_prefixes) / sizeof(char *))-1; i++) {
		size_t min_size = sizeof(ttyusb_prefixes[i]);
		if (!strncmp(ttyusb_prefixes[i], entry->d_name, min_size))
			return 1;
	}
	return 0;
}

// open the TTY-usb device:
//	- use <scandir> to find a device with a prefix given by ttyusb_prefixes
//	- returns an open fd to it
// 	- write the absolute path into <pathname> if it wasn't already
//	  given.
int open_tty(const char **portname) {
	struct dirent **name_list;
	int num_files_found;

	// find the USB
	num_files_found = scandir("/dev", &name_list, *prefix_filter, alphasort);
	if (num_files_found != 1) {
		panic("Invalid number of TTY-usb devices found.\n");
	}

	char* tty_usb_name = name_list[0]->d_name;

	// find the path name of the usb device
	char* buf = malloc(5 + strlen(tty_usb_name));

	sprintf(buf, "/dev/%s", tty_usb_name);

	*portname = buf;

	free(name_list[0]);
	free(name_list);

	// return an open fd to usb
	int fd = open(*portname, O_RDWR|O_NOCTTY|O_SYNC);
	if (fd < 0) panic("Invalid file name.\n");
	return fd;
}
