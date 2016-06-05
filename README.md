# MAX31855
Utility functions for talking to a Maxim 31855 Cold-Junction Compensated Thermocouple-to-Digital Converter.

I pulled this out of another project since it was just about ready to stand alone.  These are a couple of
utility functions to talk to a Maxim 31855 Cold-Junction Compensated Thermocouple-to-Digital Converter
module.  Stick pointy metal bit into hot things, this code tells you how hot it is.

It's C code, so should run on anything, but I specifically tested it on an AVR ATTiny861.
The SPI functions referred to are custom ones that aren't released yet.  They will be eventually,
but they're not ready for prime time.  Luckily, they're nothing terribly special, I just needed a
refresher on how the SPI controller worked. :)  As long as you have a function that can pull a
slave-select line and do a SPI read, you can substitute your own functions easily enough in the
max31855_read() function.

- Jack Bradach
2016/06/05
