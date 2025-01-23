#!/bin/bash

echo "$1"
grep -rl "$1" . | xargs sed -i "" "s/$1/$2/g"
echo "$1"

# need some sort of way to get all files that the word occurs piped in to here 
grep -E "\b[^_]$1"
echo "$1"
