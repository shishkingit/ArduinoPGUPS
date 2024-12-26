SLAVE - Arduino UNO R3 (ATMEGA328P)

avr-gcc -mmcu=atmega328p -Os -Wall -o SLAVE.elf SLAVE.c -lm
avr-objcopy -j .text -j .data -O ihex SLAVE.elf SLAVE.hex

!!!NOTE: maybe COMx instead of COM7 for you USB port!!!
avrdude -F -v -p atmega328p -c arduino -P COM7 -b 115200 -D -U flash:w:SLAVE.hex:i 

---------------------------------------------------------------------------
MASTER - Arduino NANO (ATMEGA328P)

avr-gcc -mmcu=atmega328p -Os -Wall -o MASTER.elf MASTER.c -lm
avr-objcopy -j .text -j .data -O ihex MASTER.elf MASTER.hex

!!!NOTE: maybe COMx instead of COM3 for you USB port!!!
avrdude -F -v -p atmega328p -c arduino -P COM3 -b 115200 -D -U flash:w:MASTER.hex:i
---------------------------------------------------------------------------
Need to download "Arduino UNO library" For Proteus simulation

@copyright bla-bla-bla
