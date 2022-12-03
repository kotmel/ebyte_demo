Demo from ebyte module's manufacturer. Code is stripped down to modules E07-400M10S and E07-900M10S
it is ported to SDCC and Cosmic compilers, but maintained only for SDCC compiler. Necessary stm8 library functions
are copied to single [stm8l15x_stdlib.c](E15-EVB02_E07-400M10S%2F0_Project%2FSDCC%2Fstm8l15x_stdlib.c) file 
to reduce code size. Receive callback was moved to IRQ handler and code was extended for AES encryption for 16 byte
packets. Additional GPIO B0 is configured as output for logic analyzer debugging.
 
