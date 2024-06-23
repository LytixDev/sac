#!/bin/sh

# Copyright (C) 2024 Nicolai Brand 
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

filename="sac_single.h"

rm -f $filename
touch $filename

{
    printf "/* Single header library for sac: simple arena allocator */\n\n"
    cat sac.h
    echo "#ifdef SAC_IMPLEMENTATION"
    sed -n "/IMPORT START/,/\IMPORT END/p" sac.c
    sed -n "/IMPL START/,//p" sac.c
    echo "#endif /* SAC_IMPLEMENTATION */"
} >> $filename
