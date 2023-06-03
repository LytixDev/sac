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
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#define SAC_TYPEDEF
#include "../sac.h"


int main(void)
{
#ifdef SAC_BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif

    Arena arena;
    uint8_t mem[256];
    m_arena_init(&arena, mem, 256);

    int *p = m_arena_alloc(&arena, sizeof(int));
    *p = 420;

    size_t pos_before = arena.offset;

    /* start temporary arena */
    ArenaTmp tmp = m_arena_tmp_init(&arena);
    m_arena_alloc_array(&arena, char, 16);
    /* end temporary arena */
    m_arena_tmp_release(tmp);

    size_t pos_after = arena.offset;
    assert(pos_before == pos_after);

    /* continue allocing */
    int *p2 = m_arena_alloc(&arena, sizeof(int));
    *p2 = 10;

    /* make sure we did not override anything */
    assert(*p == 420);
    assert(*(int *)m_arena_get(&arena, 0) == 420);
}
