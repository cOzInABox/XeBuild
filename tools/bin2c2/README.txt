# bin2c2

small utility for making C type arrays from a binary file. Currently outputs
bytes in the order they appear in the file (big endian on types larger than unsigned char)
though there is a little endian output method included it's not currently called.

Also note that the output types may be a little stale for more current standards.

## building

Currently only setup to build with make via makefile. Portable.

## license

see [license.txt](license.txt) for more information.

---

[cOz]
