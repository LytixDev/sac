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
#include <stdio.h>
#include <string.h>

#define SAC_IMPLEMENTATION
#include "m_arena.h"

int main()
{
#ifdef BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif

    struct m_arena *arena = m_arena_init(0, 4096);

    char *str = m_arena_alloc(arena, 16);
    char *str2 = m_arena_alloc(arena, 16);
    char *str3 = m_arena_alloc(arena, 8);
    strcpy(str, "hello");
    strcpy(str2, "world");
    strcpy(str3, ":-)");
    printf("%s %s %s\n", str, str2, str3);

    m_arena_release(arena);
}
