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

struct m_arena {
    void *memory;
    size_t pos;
    size_t capacity;
};

struct m_arena *m_arena_init(size_t starting_capacity);
void *m_arena_alloc(struct m_arena *arena, size_t capacity);
void m_arena_release(struct m_arena *arena);
void m_arena_reset(struct m_arena *arena);

#endif // !SAC_H
