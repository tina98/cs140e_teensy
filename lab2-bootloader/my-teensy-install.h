#ifndef __TEENSY_INSTALL_H__
#define __TEENSY_INSTALL_H__

int die(const char* error_msg);
void delay(double seconds);
void put_uint(int fd, unsigned u);

int read_hex(const char* filename);
int parse_hex(char* buf, int *byte_count);
void hex_get_data(int addr, int len, unsigned char *bytes);
int hex_bytes_within_range(int start, int end);

int teensy_open();
int teensy_write(int fd, unsigned char *buf, int len, double timeout);
int teensy_close(int fd);

int teensy_boot(int fd, unsigned char *buf, int write_size);
void teensy_reboot();

#endif