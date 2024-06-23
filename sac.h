/*
 *  Copyright (C) 2022-2024 Nicolai Brand (lytix.dev)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. */
#ifndef SAC_H
#define SAC_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#ifndef __unix__
#  define SAC_BAD_AARCH
#endif

/* Try to define SAC_MAP_ANON */
#ifdef MAP_ANON
#  define SAC_MAP_ANON MAP_ANON
#endif /* MAP_ANON */

#ifndef SAC_MAP_ANON
#  ifdef MAP_ANONYMOUS
#    define SAC_MAP_ANON MAP_ANONYMOUS
#  endif /* MAP_ANONYMOUS */
#endif /* SAC_MAP_ANON */

#ifndef SAC_MAP_ANON
#  define SAC_BAD_AARCH
#endif /* SAC_MAP_ANON */

#ifndef SAC_DEFAULT_ALIGNMENT
#  define SAC_DEFAULT_ALIGNMENT (sizeof(void *))
#endif

/* types */
typedef struct m_arena Arena;
typedef struct m_arena_tmp ArenaTmp;

/*
 * generic memory arena that dynamically grows its committed size.
 * more complex memory arenas can be built using this as a base.
 */
struct m_arena {
    uint8_t *memory;    // the backing memory
    size_t offset;      // first unused position in the backing memory
    bool is_dynamic;
    union {
        size_t max_pages;
        size_t backing_length;
    };
    size_t page_size;
    size_t pages_commited;   // how much of the backing memory is acutally "backing"
};

struct m_arena_tmp {
    struct m_arena *arena;
    size_t offset;
};


/* functions */
void m_arena_init(struct m_arena *arena, void *backing_memory, size_t backing_length);
void m_arena_init_dynamic(struct m_arena *arena, size_t starting_pages, size_t max_pages);
void m_arena_release(struct m_arena *arena);

void *m_arena_alloc_internal(struct m_arena *arena, size_t size, size_t align, bool zero);
#define m_arena_alloc(arena, size) m_arena_alloc_internal(arena, size, SAC_DEFAULT_ALIGNMENT, false)
#define m_arena_alloc_zero(arena, size) m_arena_alloc_internal(arena, size, SAC_DEFAULT_ALIGNMENT, true)

void m_arena_clear(struct m_arena *arena);
void *m_arena_get(struct m_arena *arena, size_t byte_idx);

#define m_arena_alloc_array(arena, type, count) (type *)m_arena_alloc((arena), sizeof(type) * (count))
#define m_arena_alloc_array_zero(arena, type, count) (type *)m_arena_alloc_zero((arena), sizeof(type) * (count))
#define m_arena_alloc_struct(arena, type) (type *)m_arena_alloc((arena), sizeof(type))
#define m_arena_alloc_struct_zero(arena, type) (type *)m_arena_alloc_zero((arena), sizeof(type))
#define m_arena_gett(arena, idx, type) (type *)m_arena_get((arena), sizeof(type) * (idx))


struct m_arena_tmp m_arena_tmp_init(struct m_arena *arena);
void m_arena_tmp_release(struct m_arena_tmp tmp);
/* cursed */
#define ARENA_TMP(___arena) for (size_t ___i = 0, ___offset = (___arena)->offset; ___i == 0; ___i += 1, (___arena)->offset = ___offset)

#endif /* !SAC_H */
