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
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include "sys/mman.h"
#include "sac.h"

/* internal functions */
static bool m_arena_commit(struct m_arena *arena, size_t commit)
{
    assert(arena->committed != SAC_NOT_MANAGE);

    /* no more space to commit */
    if (arena->pos + commit > arena->capacity)
        return false;

    int rc = mprotect(arena->memory + arena->committed, commit, PROT_READ | PROT_WRITE);
    if (rc == -1) {
        //fprintf(stderr, "sac: committing memory failed in file %s on line %d\n", __FILE__, __LINE__);
        //exit(1);
        return false;
    }

    arena->committed += commit;
    return true;
}

static bool m_arena_ensure_commited(struct m_arena *arena)
{
    if (arena->pos > arena->capacity) {
        //fprintf(stderr, "sac: arena full!\n");
        //exit(1);
        false;
    }

    if (arena->committed == SAC_NOT_MANAGE || arena->committed >= arena->pos)
        return true;

    /*
     * will only be called if pos < capacity, so we the case where we don't manage the memory,
     * i.e committed = SAC_NOT_MANAGE, will never enter here
     */
    size_t max_commitable = arena->capacity - arena->committed;
    size_t delta = arena->pos - arena->committed;
    if (delta > max_commitable)
        return false;

    /* commit as much as we need + the default, or just the default if its sufficient */
    size_t to_commit = (delta > SAC_DEFAULT_COMMIT_SIZE ? delta + SAC_DEFAULT_COMMIT_SIZE : SAC_DEFAULT_COMMIT_SIZE);
    if (to_commit > max_commitable)
        to_commit = max_commitable;

    m_arena_commit(arena, to_commit);
    return m_arena_ensure_commited(arena);
}

/*
 * stolen from:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */
static bool is_power_of_two(uintptr_t x)
{
    return (x & (x - 1)) == 0;
}

/*
 * slightly modified, but mostly stolen from:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */
static uintptr_t align_forward(uintptr_t ptr, size_t align)
{
    assert(is_power_of_two(align));

    uintptr_t p = ptr;
    uintptr_t a = (uintptr_t)align;
    uintptr_t modulo = p & (a - 1);

    if (modulo != 0)
        p += a - modulo;

    return p;
}


/* SAC functions */
void m_arena_init(struct m_arena *arena, void *backing_memory, size_t backing_length)
{
    assert(backing_memory != NULL && backing_length > 0);

    arena->memory = backing_memory;
    arena->capacity = backing_length;
    arena->pos = 0;
    arena->committed = SAC_NOT_MANAGE;
}

void m_arena_init_dynamic(struct m_arena *arena, size_t capacity, size_t starting_committed)
{
    assert(capacity >= starting_committed);

    arena->capacity = (capacity == 0 ? SAC_DEFAULT_CAPACITY : capacity);

#ifdef linux
    arena->memory = mmap(NULL, arena->capacity, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
#elif defined __unix__
    /* 
     * apparently MAP_ANONYMOUS is a linux specific feature.
     * this is the POSIX compliant way of doing it.
     */
    int fd = open("/dev/zero", O_RDWR);   
    if (fd == -1) {
        fprintf(stderr, "sac: map failed in file %s on line %d\n", __FILE__, __LINE__);
        exit(1);
    }

    arena->memory = mmap(NULL, arena->capacity, PROT_NONE, MAP_PRIVATE, fd, 0);
    if (arena->memory == MAP_FAILED) {
        fprintf(stderr, "sac: map failed in file %s on line %d\n", __FILE__, __LINE__);
        exit(1);
    }
#endif

    arena->pos = 0;
    arena->committed = 0;
    m_arena_commit(arena, starting_committed);
}

void m_arena_release(struct m_arena *arena)
{
    assert(arena != NULL);

    /* the implementation does not manage the backing memory */
    if (arena->committed == SAC_NOT_MANAGE)
        return;

    munmap(arena->memory, arena->capacity);
}

/*
 * heavily modified, but inspired by:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */
void *m_arena_alloc_internal(struct m_arena *arena, size_t size, size_t align, bool zero)
{
    uintptr_t curr_ptr = (uintptr_t)arena->memory + (uintptr_t)arena->pos;
    uintptr_t offset = align_forward(curr_ptr, align);
    offset -= (uintptr_t)arena->memory; /* change to relative offset */
    arena->pos += offset + size;

    bool success = m_arena_ensure_commited(arena);
    if (!success)
        return NULL;

    void *ptr = arena->memory + offset;
    if (zero)
        memset(ptr, 0, size);

    return ptr;
}

void m_arena_clear(struct m_arena *arena)
{
    arena->pos = 0;
}

void *m_arena_get(struct m_arena *arena, size_t byte_idx)
{
    if (byte_idx > arena->pos)
        return NULL;

    return arena->memory + byte_idx;
}
