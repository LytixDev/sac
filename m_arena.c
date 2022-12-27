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
#include <assert.h>

#include "m_arena.h"


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
    if (arena->pos > arena->capacity) {
        arena->capacity = arena->capacity * 2;
        arena->memory = realloc(arena->memory, arena->capacity);
    }
    return p;
}

void m_arena_release(struct m_arena *arena)
{
    assert(arena != NULL);

    free(arena->memory);
    free(arena);
}
