# 42-malloc

A minimal `malloc`/`free`/`realloc` implementation for macOS, built as a shared library using `mmap`-allocated zones.

## Overview

The allocator manages memory in three zone sizes:

| Zone   | Max allocation | Zone size (pages) |
|--------|---------------|-------------------|
| TINY   | ≤ 64 bytes    | 32                |
| SMALL  | ≤ 1024 bytes  | 128               |
| LARGE  | > 1024 bytes  | 256               |

The actual zone size in bytes is `page_size × zone_pages` (e.g., 512 KB for a TINY zone on a 16 KB page system).

## Architecture

```
g_malloc (t_malloc)
 ├── tiny_zones  → Zone → blocks
 ├── small_zones → Zone → blocks
 └── large_zones → Zone → blocks
```

Each zone is a contiguous `mmap`'d region containing a `t_zone` header followed by a linked list of `t_block` entries. Memory is returned 16-byte aligned.

## Building

```bash
make          # builds libft_malloc_<arch>_<os>.so and symlink
make clean    # removes object files
make fclean   # full clean
make re       # full rebuild
make test     # build and run the test suite
```

The library is named `libft_malloc_$(uname -m)_$(uname -s).so` (e.g., `libft_malloc_arm64_Darwin.so`).

## Testing

### Test suite

```bash
make test
```

### Custom test programs

Link your test binary directly against the `.so`:

```bash
cc test.c libft_malloc_arm64_Darwin.so -o test
./test
```

Or use the provided script:

```bash
./test_show_alloc.sh
```

**Note:** `DYLD_INSERT_LIBRARIES` does not work for interposing `malloc` on macOS Sequoia+. Always link directly.

## API

```c
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);   // prints allocated blocks by zone
void check_leaks(void);      // prints total leaked bytes
```

### `show_alloc_mem()` output format

```
TINY : 0x102d58000
0x102d58038 - 0x102d58078 : 64 bytes
SMALL : 0x107884000
0x107884038 - 0x107884238 : 512 bytes
LARGE : 0x107a84000
0x107a84038 - 0x107a9c6d8 : 100000 bytes
Total : 100576 bytes
```

## Project structure

```
├── includes/
│   └── malloc.h          # main header
├── src/
│   ├── malloc.c          # malloc implementation
│   ├── free.c            # free + check_leaks
│   ├── realloc.c         # realloc implementation
│   ├── show_alloc_mem.c  # memory dump
│   ├── helper.c          # zone creation, alignment, type helpers
│   ├── zone_cleanup.c    # empty zone munmap
│   ├── block/
│   │   └── block_utils.c # block splitting, search, payload
│   ├── globals.c         # global state (g_malloc, mutex)
│   └── main.c            # additional test harness (realloc tests)
├── libft/                # libft utility library
├── test_malloc.c         # unit test suite
├── test_show_alloc.sh    # show_alloc_mem demo script
└── Makefile
```

## Implementation details

- **Allocation**: `malloc` searches existing zones for a free block large enough. If none is found, a new zone is `mmap`'d and the first block is split to the requested (aligned) size.
- **Free**: Marks the block as free, then coalesces adjacent free blocks. If the zone becomes completely free, it is `munmap`'d.
- **Realloc**: Grows/shrinks in place if space allows; otherwise allocates a new block, copies data, and frees the old one.
- **Thread safety**: A single `pthread_mutex_t` protects all operations.
- **Alignment**: All allocations are rounded up to 16 bytes.
