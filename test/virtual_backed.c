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


void commit_more(void)
{
    Arena arena;
    m_arena_init_dynamic(&arena, 1, 8);
    m_arena_alloc(&arena, arena.page_size);

    uint8_t *str = m_arena_alloc(&arena, 32);
    memcpy(str, "Hello", 5);
    assert(str[0] == 'H' && str[1] == 'e' && str[2] == 'l' && str[3] == 'l' && str[4] == 'o');

    m_arena_alloc(&arena, arena.page_size);

    m_arena_release(&arena);
}

void test(void)
{
#ifdef SAC_BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif

    Arena arena;
    m_arena_init_dynamic(&arena, 1, 2);

    uint8_t *str = m_arena_alloc(&arena, 8);
    uint8_t *str2 = m_arena_alloc(&arena, 8);
    memcpy(str, "Hello", 5);
    memcpy(str2, "World", 5);

    assert(str[0] == 'H' && str[1] == 'e' && str[2] == 'l' && str[3] == 'l' && str[4] == 'o');
    assert(str2[0] == 'W' && str2[1] == 'o' && str2[2] == 'r' && str2[3] == 'l' && str2[4] == 'd');

    m_arena_release(&arena);
}

int main(void)
{
    test();
    commit_more();
}
