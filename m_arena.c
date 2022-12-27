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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "m_arena.h"


/* internal functions */
#define SAC_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

static void *sac_internal_realloc(void *p, size_t new_size)
{
    void *res = realloc(p, new_size);
    if (res == NULL) {
        fprintf(stderr, "sac: realloc failed\n");
        exit(1);
    }

    return res;
}

static void m_arena_ensure_capacity(struct m_arena *arena)
{
    if (arena->pos > arena->capacity) {
        arena->capacity = SAC_GROW_CAPACITY(arena->capacity);
        sac_internal_realloc(arena->memory, arena->capacity);
    }
}

/* functions */
struct m_arena *m_arena_init(size_t starting_capacity)
{
    assert(starting_capacity > 0);

    struct m_arena *arena = malloc(sizeof(struct m_arena));
    arena->memory = malloc(starting_capacity);
    arena->pos = 0;
    arena->capacity = starting_capacity;

    return arena;
}

void *m_arena_alloc(struct m_arena *arena, size_t capacity)
{
    assert(arena != NULL);
    assert(capacity > 0);

    void *p = arena->memory + arena->pos;
    arena->pos += capacity;
    m_arena_ensure_capacity(arena);
    return p;
}

void *m_arena_alloc_zero(struct m_arena *arena, size_t capacity)
{
    assert(arena != NULL);
    assert(capacity > 0);

    void *p = arena->memory + arena->pos;
    arena->pos += capacity;
    m_arena_ensure_capacity(arena);
    memset(p, 0, capacity);
    return p;
}

void m_arena_release(struct m_arena *arena)
{
    assert(arena != NULL);

    free(arena->memory);
    free(arena);
}

void m_arena_reset(struct m_arena *arena)
{
    arena->pos = 0;
}
