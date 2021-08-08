#!/usr/bin/python
#https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv5header

import sys

if len(sys.argv) == 1 :
	exit("no file")



file = open(sys.argv[1], 'rb')

print(f"opening {sys.argv[1]}")



print(f"\nBitmap header :")

print(f"\tSignature : {file.read(2).decode('ascii')}")

print(f"\tFile size : {int.from_bytes(file.read(4), 'little')} bytes")

print(f"\tReserved 1 : {int.from_bytes(file.read(2), 'little')}    "\
	  f"\tReserved 2 : {int.from_bytes(file.read(2), 'little')}")

OFFSET = int.from_bytes(file.read(4), 'little')
print(f"\tOffset to pixels : {OFFSET} bytes")



print(f"\nDIB header :")

HEADER_SIZE = int.from_bytes(file.read(4), 'little')
print(f"\tHeader size : {HEADER_SIZE} bytes")

if HEADER_SIZE != 124 :
	print(f"/!\\ NOT A V5 HEADER")

print(f"\tWidth : {int.from_bytes(file.read(4), 'little')} px")
print(f"\tHeight : {int.from_bytes(file.read(4), 'little')} px")

print(f"\tPlanes : {int.from_bytes(file.read(2), 'little')}    "\
	  f"\tDepth : {int.from_bytes(file.read(2), 'little')} bits")

print(f"\tCompression : {int.from_bytes(file.read(4), 'little')}")

IMAGE_SIZE = int.from_bytes(file.read(4), 'little')
print(f"\tImage size : {IMAGE_SIZE} bytes")

print(f"\tH. res : {int.from_bytes(file.read(4), 'little')} px/m")
print(f"\tV. res : {int.from_bytes(file.read(4), 'little')} px/m")


COLORS = int.from_bytes(file.read(4), 'little')
print(f"\tColors in table : {COLORS}")
print(f"\tImportant colors : {int.from_bytes(file.read(4), 'little')}")

print(f"\tR bitmask : 0x{file.read(4).hex()}")
print(f"\tG bitmask : 0x{file.read(4).hex()}")
print(f"\tB bitmask : 0x{file.read(4).hex()}")
print(f"\tA bitmask : 0x{file.read(4).hex()}")

print(f"\tColor space type : {file.read(4)[::-1].decode('ascii')}") #little endian descriptor ("Win ", "RGBs", ...)

#36 bytes long used for calibrated rgb. 3 ints = 12 bytes for each color --> 36 bytes total
print(f"\tColor space endpoints : {[tuple([file.read(4).hex() for i in range(3)]) for j in range(3)]}")	


print(f"\tR gamma : {int.from_bytes(file.read(4), 'little')}")
print(f"\tG gamma : {int.from_bytes(file.read(4), 'little')}")
print(f"\tB gamma : {int.from_bytes(file.read(4), 'little')}")


INTENTS = [
   "LCS_GM_ABS_COLORIMETRIC = 0x00000008	Match	Maintained white point",
   "LCS_GM_BUSINESS = 0x00000001	Graphic	Maintain saturation",
   "LCS_GM_GRAPHICS = 0x00000002	Proof	Maintain colors",
   "LCS_GM_IMAGES   = 0x00000004	Picture	Maintain contrast",
   ]
print(f"\tIntent : {INTENTS[int.from_bytes(file.read(4), 'little')]}")


ICC_DATA = int.from_bytes(file.read(4), 'little')
ICC_SIZE = int.from_bytes(file.read(4), 'little')
print(f"\tICC data : {ICC_DATA}")
print(f"\tICC size : {ICC_SIZE}")

print(f"\tReserved : 0x{file.read(4).hex()}") #must be 0

print(f"\tColor table : {[file.read(4).hex() for i in range(COLORS)]}")



file.seek(OFFSET)

print(f"\nPixel array :")

# print(file.read(IMAGE_SIZE).hex(sep=' '))
print("[...]")



print(f"\nICC profile :")

file.seek(ICC_DATA + 14) #add bitmap header offset..

print(f"\tColor table : {[file.read(4).hex() for i in range(ICC_SIZE)]}")




print("end !")

file.close()