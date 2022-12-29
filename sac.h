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

#include <stdlib.h>

#if !(defined __x86_64__ && defined linux)
#       define SAC_BAD_AARCH
#endif

#define GB_SIZE_T(x) ((size_t)(x) << 30)
#define SAC_DEFAULT_CAPACITY GB_SIZE_T(4)


/* types */
/*
 * generic memory arena that dynamically grows its commited size.
 * more complex memory arenas can be built using this as a base.
 */
struct m_arena {
    void *memory;
    size_t pos;
    size_t capacity;
    size_t committed;
};


/* functions */
struct m_arena *m_arena_init(size_t capacity, size_t starting_commited);
void m_arena_release(struct m_arena *arena);

void *m_arena_alloc(struct m_arena *arena, size_t size);
void *m_arena_alloc_zero(struct m_arena *arena, size_t size);
int m_arena_free(struct m_arena *arena, size_t size);

void m_arena_clear(struct m_arena *arena);

#define m_arena_alloc_array(arena, type, count) (type *)m_arena_alloc((arena), sizeof(type) * (count))
#define m_arena_alloc_array_zero(arena, type, count) (type *)m_arena_alloc_zero((arena), sizeof(type) * (count))
#define m_arena_alloc_struct(arena, type) m_arena_alloc_array((arena), (type), 1)
#define m_arena_alloc_struct_zero(arena, type) m_arena_alloc_array_zero((arena), (type), 1)


#endif /* !SAC_H */


#ifdef SAC_IMPLEMENTATION

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include "sys/mman.h"

/* internal functions */
static void m_arena_commit(struct m_arena *arena, size_t commit)
{
    int rc = mprotect(arena->memory + arena->committed, commit, PROT_READ | PROT_WRITE);
    if (rc == -1) {
        fprintf(stderr, "sac: committing memory failed in file %s on line %d\n", __FILE__, __LINE__);
        exit(1);
    }
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
struct m_arena *m_arena_init(size_t capacity, size_t starting_committed)
{
    assert(starting_committed > 0);

    struct m_arena *arena = malloc(sizeof(struct m_arena));
    arena->capacity = capacity == 0 ? SAC_DEFAULT_CAPACITY : capacity;

#ifdef linux
    arena->memory = mmap(NULL, arena->capacity, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
#elif defined __unix__
    /* 
     * apparently MAP_ANONYMOUS is a linux specific feature.
     * this is the POSIX compliant way of doing it.
     */
    int fd = open("/dev/zero", O_RDWR);   
    if (fd == -1)
        exit(1);

    arena->memory = mmap(NULL, arena->capacity, PROT_NONE, MAP_PRIVATE, fd, 0);
    if (arena->memory == MAP_FAILED) {
        fprintf(stderr, "sac: map failed in file %s on line %d\n", __FILE__, __LINE__);
        exit(1);
    }
#endif

    arena->committed = 0;
    arena->pos = 0;
    m_arena_commit(arena, starting_committed);
    return arena;
}

void *m_arena_alloc(struct m_arena *arena, size_t size)
{
    assert(arena != NULL);

    void *p = arena->memory + arena->pos;
    arena->pos += size;
    m_arena_ensure_commited(arena);
    return p;
}

void *m_arena_alloc_zero(struct m_arena *arena, size_t size)
{
    assert(arena != NULL);

    void *p = arena->memory + arena->pos;
    arena->pos += size;
    m_arena_ensure_commited(arena);
    memset(p, 0, size);
    return p;
}

int m_arena_free(struct m_arena *arena, size_t size)
{
    if (size > arena->pos)
        return -1;

    arena->pos -= size;
    return 0;
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

#endif /* !SAC_IMPLEMENTATION */
