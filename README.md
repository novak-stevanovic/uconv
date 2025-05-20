# UConv

__UConv__ is a tiny C library for converting between UTF-8 and UTF-32 text. It provides functions to decode a byte sequence into Unicode code points and to encode code points back into UTF-8. It is designed with three things in mind: control, proper error handling and simplicity.

## Makefile instructions:

1. make \[LIB\_TYPE=so/ar\] \[OPT={0...3}\] - This will compile the source files(and thirdparty dependencies, if they exist) and build the .so/.a file.
2. make install \[PREFIX={prefix}\] \[LIB\_TYPE=so/ar\] - This will place the public headers inside PREFIX/include and the built .so/.a file inside PREFIX/lib.

Default options are PREFIX=/usr/local, OPT=2, LIB\_TYPE=so.
