# lunatic
A toy interpreter for lua

Supports if, else, while, function, recursion, metatable.

GC using reference counting.

Running on a register based VM.

Offers C/C++ interop using template metaprogramming; automaticaly detects C++ function signature and install it into the script engine, no wrapper needed.

Some what 2x slower than official lua.
