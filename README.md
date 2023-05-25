# sac - Simple Arena Allocator

Inspired by <a href="https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator">this article</a> by Ryan Fleury and <a href="https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/">this article</a> by gingerbill.

A generic dynamically growing arena allocator for Unix-like systems (systems that define `__unix__`). The growing strategy can take advantage of the massive virtual memory space on 64 bit systems. By default, sac reserves 2 gigabytes of virtual memory, but only commits more as its needed. This guarantees contigious virtual memory without wasting or reallocatting memory.

A sac arena doesn't need to be backed by virtual memory. You can also use a sac arena with any type of backed memory. See `examples.c` and `static_example()` (for lack of a better word).
