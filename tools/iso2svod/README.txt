# iso2svod

This source is a clean room reverse engineered implementation of the process that
occurs when installing an optical game disk to the harddrive (ITHDD)

## about

Yes, I know there are a bunch of them out there, when I wrote this the original
iso2god GUI was making images that were using a very old SVOD format implemented from
a Games On Demand container rather than the newer version implemented for ITHDD.

The newer implementation is a little tighter, saving a small amount of space and
may also account for overburned game disk sizes better.

## building

To build this project you will have to supply libiconv, libxml2, and zlib. It also
relies on having a working XeCrypt lib/implementation.

Currently only set up to build on windows/visual studio, should be fairly portable.

## license

see [license.txt](license.txt) for more information.

---

[cOz]
