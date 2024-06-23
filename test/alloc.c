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
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "../sac.h"


struct tuple_t {
    int a, b;
};

int main(void)
{
#ifdef SAC_BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif

    if (SAC_DEFAULT_ALIGNMENT != 8 || sizeof(int) != 4) {
        fprintf(stderr, "NOTE: skipping this test as it will only work for 64 bit systems");
        return 0;
    }

    Arena arena;
    uint8_t mem[256];
    m_arena_init(&arena, mem, 256);

    int *p = m_arena_alloc_zero(&arena, sizeof(int) * 3);
    assert(p[0] == 0);
    assert(p[1] == 0);
    assert(p[2] == 0);

    p[0] = 1;
    p[1] = 2;
    p[2] = 3;
    assert(p[0] == 1);
    assert(p[1] == 2);
    assert(p[2] == 3);


    /* 
     * we allocated 12 bytes earlier.
     * the next aligned pointer should be at offset 16 since its the first multiple of 8
     * this means that the internal_pos_after should be offset 16 + sizeof(struct tuple_t)
     */
    size_t internal_pos_before = arena.offset;
    struct tuple_t *tuple = m_arena_alloc_struct(&arena, struct tuple_t);
    size_t internal_pos_after = arena.offset;

    assert(internal_pos_after == 16 + sizeof(struct tuple_t));
    /* exactly same as above. 4 padding bytes should've been added */
    assert(internal_pos_after == internal_pos_before + 4 + sizeof(struct tuple_t));

    tuple->a = 1;
}
