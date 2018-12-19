#!/usr/bin/env sh
# This script does perform all linting for the repo.
# Run this from the project root.
# None of these commands should write anything to stdout; if they do, that indicates a failure.

printf 'Find source files with executable permissions\n' 1>&2
find * -perm +111 -type f | grep -Fv 'external_libraries' | \
    grep -E '\.(c|cpp|h|hpp|cc|hh|schelp|sc|scd|md|txt|css|js|html)$'

printf 'Run clang-format based on diff\n' 1>&2
git show --pretty='format:' -U0 | clang-format-diff.py -p1 -style file

printf 'Lint CMake files\n' 1>&2
git show --pretty='format:' --name-only | grep CMakeLists.txt | \
    xargs cmakelint --filter=-linelength 2>/dev/null

printf 'Find trailing whitespace in any file in HelpSource, SCClassLibrary, testsuite, and examples\n' 1>&2
# rg is much faster than grep, so use it if it's available
if which -s rg; then
    rg -nHF "\s$" HelpSource SCClassLibrary testsuite examples
else
    grep -nIHFr "\s$" HelpSource SCClassLibrary testsuite examples
fi

printf 'Find leading whitespace violations in .sc/.scd files\n' 1>&2
find SCClassLibrary testsuite examples -type f \( -name '*.sc' \) -or \( -name '*.scd' \) -print0 | \
    xargs -0 grep -nIH "^\t* \t*\([^*]\|$\)"

exit 0
