#!/bin/bash

set -o errexit
shopt -s globstar

dir="$1"
config="$dir"/.editorconfig

# class library
lintspaces -e "$config" "$dir"/SCClassLibrary/**/*.sc
