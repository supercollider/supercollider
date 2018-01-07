#!/bin/sh
# Converts a changelog in md format to schelp format
# Usage: changelog_to_schelp.sh <input> <output> <version>
# Where <input> is in md format and <output> is in schelp
# If <version> is not provided, it will find the first match of 3.# in the file and use that
# You still have to add a related:: tag.
#
# Brian Heim, 2017-12-26

if [[ -z "$1" || -z "$2" ]]; then
    echo "Usage: changelog_to_schelp.sh <input> <output> <version>"
    echo "\t<input> must exist, <output> must not exist"
    echo "\tif <version> is not provided, this script will attempt to pull it from <input>"
    exit 1
elif [[ ! -e "$1" ]]; then
    echo "File '$1' does not exist"
    exit 1
elif [[ -e "$2" ]]; then
    echo "File '$2' exists"
    exit 1
fi

LOG=`cat "$1"`

VERSION="$3"
if [[ -z "$VERSION" ]]; then
    echo "No version provided, attempting to grab version from log"
    VERSION=`echo "$LOG" | grep -m 1 -o "[[:digit:]]\+\.[[:digit:]]\+"`
    echo "Found version: $VERSION"
fi

# header
echo "\
title:: News in $VERSION
summary:: A summary of news in SC $VERSION
categories:: News
" >> "$2"

# Section headers
LOG=`echo "$LOG" | perl -0777 -pe "s/([^\n]*)\n---*/section:: \1/igs"`

# `name` -> code::name::
LOG=`echo "$LOG" | sed "s/\\\`\([^\\\`]*\)\\\`/code:: \1 ::/g"`

# **bold** -> strong:: bold ::
LOG=`echo "$LOG" | sed "s/\*\*\([^\*]*\)\*\*/strong:: \1 ::/g"`

# turn links into plain text
LOG=`echo "$LOG" | sed "s/\[\([^]]*\)\]([^)]*)/\1/g"`

# remove parenthesized text with a # in it
LOG=`echo "$LOG" | sed "s/\s\+([0-9a-zA-Z, ]*#[#0-9a-zA-Z, ]\+)//g"`

echo "$LOG" >> "$2"

exit 0
