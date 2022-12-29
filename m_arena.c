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
#include <fcntl.h>
#include "sys/mman.h"

#include "m_arena.h"


/* internal functions */
static void m_arena_commit(struct m_arena *arena, size_t commit)
{
    mprotect(arena->memory + arena->committed, commit, PROT_READ | PROT_WRITE);
    arena->committed += commit;
}

static void m_arena_ensure_commited(struct m_arena *arena)
{
    if (arena->pos > arena->capacity) {
        fprintf(stderr, "sac: arena full!\n");
        exit(1);
    }
    if (arena->pos > arena->committed) {
        /* attempt to double the commited capacity */
        size_t to_commit = arena->committed;
        size_t max_commitable = arena->capacity - arena->committed;
        if (to_commit > max_commitable)
            to_commit = max_commitable;
        m_arena_commit(arena, to_commit);
    }
}

/* functions */
struct m_arena *m_arena_init(size_t starting_commited)
{
    assert(starting_commited > 0);

    struct m_arena *arena = malloc(sizeof(struct m_arena));

    int fd = open("/dev/zero", O_RDWR);   
    if (fd == -1)
        exit(1);

    arena->capacity = GB_SIZE_T(4);
    arena->memory = mmap(NULL, arena->capacity, PROT_NONE, MAP_PRIVATE, fd, 0);
    if (arena->memory == MAP_FAILED) {
        fprintf(stderr, "sac: map failed in file %s on line %d\n", __FILE__, __LINE__);
        exit(1);
    }

    arena->committed = 0;
    arena->pos = 0;
    m_arena_commit(arena, starting_commited);
    return arena;
}

void *m_arena_alloc(struct m_arena *arena, size_t size)
{
    assert(arena != NULL);
    assert(size > 0);

    void *p = arena->memory + arena->pos;
    arena->pos += size;
    m_arena_ensure_commited(arena);
    return p;
}

void *m_arena_alloc_zero(struct m_arena *arena, size_t capacity)
{
    assert(arena != NULL);
    assert(capacity > 0);

    void *p = arena->memory + arena->pos;
    arena->pos += capacity;
    m_arena_ensure_commited(arena);
    memset(p, 0, capacity);
    return p;
}

void m_arena_release(struct m_arena *arena)
{
    assert(arena != NULL);
    munmap(arena->memory, arena->capacity);
    free(arena);
}

void m_arena_reset(struct m_arena *arena)
{
    arena->pos = 0;
}
