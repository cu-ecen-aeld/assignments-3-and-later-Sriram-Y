#!/bin/bash

# Check for arguments
if [ $# -ne 2 ]; then
    echo "Error: Two arguments required: <filesdir> <searchstr>"
    exit 1
fi

filesdir=$1
searchstr=$2

# Valid directory check
if [ ! -d "$filesdir" ]; then
    echo "Error: $filesdir is not a directory"
    exit 1
fi

# Find all files in current and directories below
numoffiles=$(find "$filesdir" -type f | wc -l)
# Search for string in files and return how many times the string occurs in the files
numofmatches=$(grep -r "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $numoffiles and the number of matching lines are $numofmatches"

exit 0
