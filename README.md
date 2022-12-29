# sac - Simple Arena Allocator

Inspired by <a href="https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator">this article</a> by Ryan Fleury.

A generic dynamically growing arena allocator for Unix-like systems. The growing strategy can take advantage of the massive virtual memory space on 64 bit systems. By default, sac reserves 4 gigabytes of virtual memory, but only committs more memory as its needed. This guarantees contigious virtual memory without wasting or reallocatting memory.

### Setup
Include the header file into your C project:
```c
#define SAC_IMPLEMENTATION /* defining the implemention once is sufficient */
#include "sac.h"
```
