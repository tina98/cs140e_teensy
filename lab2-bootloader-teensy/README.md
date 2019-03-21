# Lab02-bootloader

See the README one level up about where to find the reference manual and schematics for the board.

First, we have to send the code in teensy-side over USB (teensy command line) so it's ready to receive new code, then run the unix-side code to send over whatever .bin file. It should look something like this:

> teensy teensy-side/t_bootloader.gcc.thumb/hex
unix-side/my-install blink.bin

The blink.bin code is just the code from lab01 to blink the LED. 


## Code logic
Everything is very similar to [CS140e's bootloader code](https://github.com/dddrrreee/cs140e-win19/tree/master/labs/lab2-bootloader), with some different registers and minor differences. The biggest thing is that the UART protocol for the Teensy seems to be more buggy, so when sending over the code, instead of sending everything all at once from the Unix side, we're waiting for ACK from the Teensy. Additionally, on the Teensy, instead of storing the code at ARMBASE, we're just storing it in a defined array (code[]) then executing it. Obviously, this stops working when there's larger pieces of code to send over, but works fine for now the small amount of blink LED code.

### Unix side (simple-boot.c)
1. Send SOH, file size, and checksum of the entire file.
2. Verify echoed SOH, checksum of nbytes file size, and echoed file checksum.
3. Send ACK and expect one back.
4. Loop to send binary code and receive ACK for every int.
5. Send EOT.
6. Expect ACK and finish.

To debug, I also wrote a simple bit_addition function where it just sends over a variable that the Teensy increments, then the Unix increment, and it just goes back and forth.

### Teensy side (t_bootloader.c)
1. Wait for SOH, size, cksum from unix side.
2. Echo SOH, checksum(size), cksum back.
3. Wait for ACK.
4. Read the bytes, one at a time, copy them to code[]. Send ACK back.
5. Verify file checksum.
6. Send ACK back.
7. jump to code[].

To debug, I also wrote a simple bit_banging function where it just waits for a variable from the computer, then increment it and send it back to the computer. The Unix side also increments and it just goes back and forth.

