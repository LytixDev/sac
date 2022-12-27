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

#include "m_arena.h"


int main()
{
    struct m_arena *arena = m_arena_init(32);

    char *str = m_arena_alloc(arena, 16);
    char *str2 = m_arena_alloc(arena, 16);
    strcpy(str, "hello");
    strcpy(str2, "world!");
    printf("%s %s\n", str, str2);

    m_arena_release(arena);
}
