/*
 *  Copyright (C) 2022 Nicolai Brand 
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef SAC_H
#define SAC_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#if !(defined __x86_64__ && defined __unix__)
#       define SAC_BAD_AARCH
#endif

#define KB_SIZE_T(x) ((size_t)(x) << 10)
#define GB_SIZE_T(x) ((size_t)(x) << 30)
#define SAC_DEFAULT_CAPACITY GB_SIZE_T(2)
#define SAC_DEFAULT_COMMIT_SIZE KB_SIZE_T(8)

#ifdef SAC_TYPEDEF
typedef struct m_arena Arena;
typedef struct m_arena_tmp ArenaTmp;
#endif /* SAC_TYPEDEF */

#ifndef SAC_DEFAULT_ALIGNMENT
#define SAC_DEFAULT_ALIGNMENT (sizeof(void *))
#endif

/* implementation does not manage the backing memory */
#define SAC_NOT_MANAGE SIZE_MAX


/* types */
/*
 * generic memory arena that dynamically grows its committed size.
 * more complex memory arenas can be built using this as a base.
 */
struct m_arena {
    uint8_t *memory;    // the backing memory
    size_t offset;      // first unused position in the backing memory
    size_t capacity;    // the maximum capacity of the backing memory
    size_t committed;   // how much of the backing memory is acutally "backing"
};

struct m_arena_tmp {
    struct m_arena *arena;
    size_t offset;
};


/* functions */
void m_arena_init(struct m_arena *arena, void *backing_memory, size_t backing_length);
void m_arena_init_dynamic(struct m_arena *arena, size_t capacity, size_t starting_committed);
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

#endif /* !SAC_H */

