UART -> DMX
=============================================================
________________________________________________________________________
|-\____>22bits_________/---->2bits------\_frameStart/--\_frame1/--\_....|
|_______________________________________________________________________|

DMX is very similar to UART except for the first 24 bits, in our example, we
were able to resolve this problem by switching the baud rate from 250kbits/s to
90909 bits/s for the start of the message.

For the program to run smoothly, before switching the baud rate from 90909 to
250kbits/s we need to wait with a "for" loop for at least 2500 loops.

The output of the TX signal is the gpio 4.

Connections :
|----------|-----------------|
|  ESP32   |     sparkfun    |
|----------|-----------------|
|   3V3    |   3V3 + 485/EN  |
|   GND    |       GND       |
|  GPIO 4  |     485/RX      |
|----------|-----------------|
