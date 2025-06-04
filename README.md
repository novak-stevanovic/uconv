# UConv

__UConv__ is a tiny C library for converting between UTF-8 and UTF-32 text. It provides functions to decode a byte sequence into Unicode code points and to encode code points back into UTF-8. It is designed with three things in mind: control, proper error handling and simplicity.

## Makefile instructions:

1. `make [LIB_TYPE=so/ar] [OPT={0...3}]` - This will compile the source files(and thirdparty dependencies, if they exist) and build the library file.
2. `make install [PREFIX={prefix}] [LIB_TYPE=so/ar] [PC_PREFIX={pc_prefix}]` - This will place the public headers inside _{prefix}/include_ and the built library file inside _{prefix}/lib_.

Default options are `PREFIX=/usr/local`, `OPT=2`, `LIB_TYPE=so`, `PC_PREFIX=/usr/local/lib/pkgconfig`.

## Usage instructions:

Since this "library" consists of only one header and one implementation file, it is probably easiest to just copy-paste the files into your project and compile them alongside other files. The alternative is explained below.

To use the library in your project, you must first install it. This can be done on your system - globally, or locally, inside a project that is using this library.
1. Install with desired `PREFIX` and `PC_PREFIX`.
2. Compile your project with cflags: `pkfconf --cflags uconv` and link with flags: `pkfconf --libs uconv`. For this to work, make sure that pkgconf seaches in the directory `PC_PREFIX` when using pkgconfig.


