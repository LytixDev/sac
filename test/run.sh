#!/bin/sh

# Copyright (C) 2023 Nicolai Brand (https://lytix.dev)
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

successes=0
mem_leak_option="$1"
n_tests="$(find . -maxdepth 1 -type f -name '*.c' | wc -l)"

for file in *.c
do
    gcc -g -fsanitize=address -fsanitize=undefined -Wall -Wpedantic -Wextra -Werror "$file" ../sac.c
    [ "$mem_leak_option" = "-m" ] && (valgrind -s ./a.out >/dev/null)
    if ./a.out
    then
        printf "%s \033[0;32mPASSED\033[0m\n" "$file"
        successes=$((successes + 1))
    else
        printf "%s \033[0;31mFAILED\033[0m\n" "$file"
    fi
done

rm -f a.out

echo "---------------------"
echo "$successes"/"$n_tests" tests passed
