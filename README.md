# cs140e_teensy
I reimplemented some of the CS140E labs using the Teensy 3.2. The main references I used are K20 sub-family reference (https://www.pjrc.com/teensy/K20P64M72SF1RM.pdf), the Teensy website with schematics and technical specifications (https://www.pjrc.com/), and David Welch's Teensy examples. 

To run these labs, I had an LED connected to pin 12 (and ground), and I also connected to pins 0/1 for the UART RX/TX. The Teensy has a convenient LED by pin 13 so you don't necessarily have to hook up additional LEDs if just one is enough. Additionally, to actually send code to the Teensy, I used the provided bootloader: https://www.pjrc.com/teensy/loader_cli.html. The command is:
>teensy_loader_cli -mmcu=mk20dx256 -w [hex_file].hex
To make things easier, in my .bash_profile, I made an alias that points to the location of the command line loader so all I have to run is 
>teensy [hex_file].hex
To add the alias, you have to put the following in your .bashrc or .bash_profile:
>alias teensy="/Users/christinali/Documents/Stanford/JuniorYear/WinterQtr/CS140E/Teensy3x/teensy_loader_cli -mmcu=mk20dx256 -w -v"



##Lab01-blinker

##Lab02-bootloader

##Lab03-uart

##Lab07-timer-interrupts
