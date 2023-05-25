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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SAC_TYPEDEF
#include "sac.h"

Arena *context = NULL;

void context_foo(void)
{
    char *str = m_arena_alloc(context, 16);
    char *str2 = m_arena_alloc(context, 16);
    char *str3 = m_arena_alloc(context, 32);
    strcpy(str, "Hello");
    strcpy(str2, "World");
    strcpy(str3, "from context :-)");

    printf("%s %s %s\n", str, str2, str3);
}

void context_example(void)
{
    Arena *arena = malloc(sizeof(Arena));;
    uint8_t mem[256];
    m_arena_init(arena, mem, 256);
    context = arena;
    context_foo();
    // not needed as the arenas backed memory has not been allocated manually anywhere
    m_arena_release(arena);
}

void static_example(void)
{
    Arena arena;
    uint8_t mem[256];
    m_arena_init(&arena, mem, 256);

    char *str = m_arena_alloc(&arena, 8);
    char *str2 = m_arena_alloc(&arena, 8);
    char *str3 = m_arena_alloc(&arena, 16);
    strcpy(str, "Hello");
    strcpy(str2, "World");
    strcpy(str3, "from static :d");

    printf("%s %s %s\n", str, str2, str3);
}


void dynamic_example(void)
{
    Arena arena;
    m_arena_init_dynamic(&arena, SAC_DEFAULT_CAPACITY, SAC_DEFAULT_COMMIT_SIZE);

    char *str = m_arena_alloc(&arena, 8);
    char *str2 = m_arena_alloc(&arena, 8);
    char *str3 = m_arena_alloc(&arena, 16);
    strcpy(str, "Hello");
    strcpy(str2, "World");
    strcpy(str3, "from dynamic :3");

    printf("%s %s %s\n", str, str2, str3);

    m_arena_release(&arena);
}

int main(void)
{
#ifdef SAC_BAD_AARCH
    fprintf(stderr, "may not work properly");
#endif
    
    dynamic_example();
    static_example();
    context_example();
}
