input on pg103
output on pg106

Extended instructions are of 32bit:
6bit opcode + 6bit flags+ 20 bit address

made all the modifications in pass1, now add separate symtabs for each section
then do pass 2 - modification, define, refer records
then make linker-loader.

you've hardcoded the no of sections. make sure to make that dynamic in the end. 

also find program size of each section