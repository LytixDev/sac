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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdio.h>
#include "../sac.h"


int main(void)
{
#ifdef SAC_BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif

    Arena arena;
    uint8_t mem[256];
    m_arena_init(&arena, mem, 256);

    int *p = m_arena_alloc(&arena, sizeof(int) * 3);
    p[0] = 1;
    p[1] = 2;
    p[2] = 3;

    /* dummy alloc */
    m_arena_alloc(&arena, 8);

    int *list_ptr = m_arena_get(&arena, 0);
    assert(list_ptr[0] == 1);
    assert(list_ptr[1] == 2);
    assert(list_ptr[2] == 3);
}
